
/**************************** lua_io_wrappers.c *************************/
/* NBEE Embedded Systems S.L.                                           */
/* All rights reserved.													*/
/*																		*/
/* Redistribution and use in source and binary forms, with or without	*/
/* modification, are permitted provided that the following conditions	*/
/* are met:																*/
/* 1. This code is distributed only with Espardino (tm) products, or    */
/*    derivative final products not intended as development systems.    */
/* 2. Redistributions of source code must retain the above copyright	*/
/*   notice, this list of conditions and the following disclaimer.		*/
/* 3. Redistributions in binary form must reproduce the above copyright	*/
/*   notice, this list of conditions and the following disclaimer in the*/
/*   documentation and/or other materials provided with the 			*/
/*   distribution.														*/
/* 4. The name of NBEE Embedded Systems S.L. or its contributors may    */
/* not be used to endorse or promote products derived from this software*/
/* without specific prior written permission.							*/
/*																		*/
/* THIS SOFTWARE IS PROVIDED BY NBEE Embedded Systems S.L "AS IS" AND 	*/
/* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE*/
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR	*/
/* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NBEE EMBEDDED SYSTEMS  BE	*/
/* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	*/
/* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	*/
/* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 		*/
/* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,*/
/* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE */
/* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 	*/
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.					*/
/************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <vcom.h>
#include <string.h>
#include <micro214x.h>
#include "lua_io_wrappers.h"

/* from miniprintf.c */
int print(char **out, int *varg);

FATFS* lua_fatfs;
int my_stdin_eof = 0;

void LUA_setup_fatfs(FATFS *fs)
{
  lua_fatfs = fs;
}


MY_FILE *my_fopen(char *filename,char* mode)  // /"r" /"rb"/ "w"
{
	unsigned char mode_ch = 0;
	
	FIL * f = (FIL*)malloc(sizeof(FIL));
	
	
	if ((mode[0]=='r') || (mode[1]=='r')) mode_ch = FA_READ;
	if ((mode[0]=='w') || (mode[1]=='w')) mode_ch |= FA_WRITE | FA_CREATE_ALWAYS;
	
	
	
	if (f_open(f,filename,mode_ch)!=FR_OK) return NULL;
	else 
	{
		return (MY_FILE *) f;
	}
	
}

MY_FILE *my_freopen(char *nombre, char *modo, MY_FILE *stream)
{
	my_fclose(stream);
	return my_fopen(nombre,modo);
}


int my_feof(MY_FILE *f)
{
	int res;
	
	if (f==MY_STDIN)
	{
		if (my_stdin_eof) { my_stdin_eof = 0; return 1; }
		else return 0;
	}
	
	if (
		(f==MY_STDOUT) ||
		(f==MY_STDERR)) return 0;
	
	return f_eof((FIL*)f);
}

int my_fclose(MY_FILE *f)
{
	int res;
	
	if ((f==MY_STDIN) ||
		(f==MY_STDOUT) ||
		(f==MY_STDERR)) return 0;
		
	else 
	{	
		res = f_close((FIL *)f);
		free (f);
		return res;
	}
	
}

int my_fflush(MY_FILE *f)
{
	if (f==MY_STDIN ||
		f==MY_STDOUT ||
		f==MY_STDERR) return 0;
		
	else 
	{
		return f_sync((FIL*)f);
	}
}

int my_ftell(MY_FILE *f)
{
	if (f==MY_STDIN ||
		f==MY_STDOUT ||
		f==MY_STDERR) return 0;
	
	return ((FIL*)f)->fptr;
	
}

int my_fwrite(char *buffer, int tblock, int nblock, MY_FILE *f)
{
	
	int res, written;
	unsigned int written_last;
	written = 0;
	
	
	if (f==MY_STDOUT || f==MY_STDERR)
	{
		nblock = nblock * tblock;
		while (nblock--)
		{
			VCOM_putchar_nl(*buffer++);
			written++;
		}
		
		return written;
	}
	
	if (f==MY_STDIN) return 0;
	
	
	while (nblock--)
	{
		written_last = 0;
		res = f_write((FIL*)f,buffer,tblock,&written_last);
		written +=written_last;
		if (res!=FR_OK)
		{
			return written;
		}
		buffer += tblock;
	}
	return written;

}



int my_fread(char *buffer, int tblock ,int nblock,MY_FILE *f)
{
	int res, read;
	unsigned int read_last;
	read = 0;
	int c;
	
	
	if (f==MY_STDIN)
	{
		nblock = nblock * tblock;
		while (nblock--)
		{
			c= VCOM_getchar();
			
			if (c!=4) VCOM_putchar_nl(c);
			if (c=='\r') VCOM_putchar_nl('\n');	
		
			if (c==4) /*EOT*/ {
				my_stdin_eof = 1;
				xputs("\n");
				break;
			}
			*buffer++ = c;
			
			read++;
		}
		
		return read;
	}
	
	if (f==MY_STDOUT || f==MY_STDERR) return 0;
	
	
	while (nblock--)
	{
		
		read_last = 0;
		res = f_read((FIL*)f,buffer,tblock,&read_last);
		read +=read_last;
		if (res!=FR_OK)
		{
			return read;
		}
		buffer += tblock;
	}
	return read;


}

int my_getc(MY_FILE *fil)
{
	int f;
	char b;
	char echo;
	
	if (fil==MY_STDIN ||
		fil==MY_STDOUT ||
		fil==MY_STDERR) 
	{
		
		echo= VCOM_getchar();
		VCOM_putchar_nl(echo);
		return echo;
	}
		
	if (fil==NULL) return EOF;
	
	if (my_fread(&b, 1 , 1,fil)==1) return b;
	else return EOF;

}

int my_exit(int val)
{
	xprintf("EXIT:%d\n",val);
	while(1);
}


int my_ungetc(int c,MY_FILE *fil)
{
	FIL *f;
	int dst;
	
	if (fil==MY_STDIN ||
		fil==MY_STDOUT ||
		fil==MY_STDERR ||
		fil==NULL ) return 0;
	
	f = (FIL *) fil;
	
	dst = f->fptr -1;
	if (dst<0) dst = 0;
	
	return f_lseek(f,dst);
	
	

}


char* my_fgets(char *buff,int len,MY_FILE *fil)
{
	int i = 0;
	char *p = buff;
	UINT rc;

	if (fil==MY_STDOUT || fil==MY_STDERR || fil==NULL) return NULL;
	
	while (i < len - 1) {			/* Read bytes until buffer gets filled */
		if (fil==MY_STDIN)
		{
			*p = VCOM_getchar();
			if (*p=='\r') *p='\n';
			
			if (*p==8) /*backspace */
			{
				VCOM_putchar_nl(8);
				VCOM_putchar_nl(' ');
			}
			VCOM_putchar_nl(*p);
			rc=1;
			
			
		}
		else
		{
			f_read((FIL*)fil, p, 1, &rc);
		}
		if (rc != 1) break;			/* Break when no data to read */
		//if (*p == '\r') continue;	/* Strip '\r' */
		i++;
		if (*p=='\r') { p++; break; }
		
		if (*p == '\n') { p++; break;}	/* Break when reached end of line */
		if (*p==8)
		{
			p--;
			if (p<buff) p=buff;
		}
		else
		{
			p++;
		}
		
	}
	*p = 0;
	return i ? buff : NULL;			/* When no data read (eof or error), return with error. */
}


int my_fputs(char *buffer,MY_FILE *f)
{
	int c,count;
	if (f==MY_STDIN) return 0;
	if (f==MY_STDOUT || f==MY_STDERR)
	{
			count = 0;
			while(*buffer)
			{
				VCOM_putchar_nl(*buffer);
				buffer++;
				count++;
			}
			return count;
		
	}
	else
	{
		if (f==NULL) return 0;
		return f_puts((const char*)buffer, (FIL*)f);
	}

}

int my_fputc(int c, MY_FILE *f)
{
	if (f==MY_STDIN || f==NULL) return 0;
	if (f==MY_STDOUT || f==MY_STDERR)
	{
	
		VCOM_putchar_nl(c);
		return 1;
		
	}
	else
	{
	
		return f_putc(c, (FIL*)f);
	}
}

int my_fseek(MY_FILE *f, long int offset, int mode)
{
	
	long int dir;
	
	if (f==MY_STDIN ||
		f==MY_STDOUT ||
		f==MY_STDERR || f==NULL) return 0;
	
	
	switch (mode)
	{
		case SEEK_CUR:	dir=((FIL*)f)->fptr + offset;	break;
		case SEEK_SET: dir = offset;			break;
		case SEEK_END: dir = ((FIL*)f)->fsize - offset;break;
		default: 
			dir = offset;
	}
	
	return f_lseek((FIL*)f,dir);
	
}/* SEEK_CUR, SEEK_SET, SEEK_END */ 


MY_FILE *my_f_out = MY_STDOUT;

int my_putchar(int c)
{
	return my_fputc(c,my_f_out);
}

int my_fprintF(MY_FILE *f, const char *format, ...)
{

	xprintf_output(&my_putchar);
	int *varg = (int *)(&format);
	my_f_out = f;
	return print(0, varg);
	xprintf_output(&VCOM_putchar_nl);
}

int my_printF(const char *format, ...)
{
	int *varg = (int *)(&format);
	return print(0, varg);
}

int my_sprintF(char *out, const char *format, ...)
{
	int *varg = (int *)(&format);
	return print(&out, varg);
}




