//
// Copyright (c) 2009  http://www.espardino.com
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <LPC214x.h>
#include <vcom.h>
#include <efs/efs.h>
#include <efs/ls.h>
#include <string.h>
#include <micro214x.h>
#include <stdio.h>
#include <stdlib.h>

void print_banner(void);

void print_banner(void)
{
	xprintf("\n");
	xprintf("http://www.espardino.com micro214x EFSL microSD test\n");
	xprintf("----------------------------------------------------\n");
	xprintf("\n");
}

int main(void)
{
	char str[256],k;
	int res;
	
	EmbeddedFileSystem efs;
	EmbeddedFile filer;
	DirList list;

	IO_init(FAST_IO);
	LEDS_init();	/* init the LEDs for this board  */
	VCOM_init();	/* init the USB Virtual COM port */
	   	
	xprintf_output(&VCOM_putchar_nl);	/* define the output for the xprintf/puts */

	LEDS_on(LED3);
	
	
	while (1) {
	
		k = VCOM_getchar();
		
		LEDS_on(LED1);
		
		print_banner();
		
		/* we init the EFS filesystem libraries */
		if ( ( res = efs_init( &efs, 0 ) ) != 0 ) {
			xprintf("EFS: Error initializing access to microSD slot!\n");
			continue;
		}
		
		/* open the root directory */
		if (ls_openDir(&list, &(efs.myFs), "/") != 0)
		{
			xprintf("EFS: Error opening '/' dir for read\n");
			continue;
		}

		xprintf("\nDirectory /:\n");
		
		/* print all file entries */
		while (ls_getNext(&list)==0)
		{

		 	format_file_name(str, (char*)list.currentEntry.FileName);
			xprintf("%s\n",str);
		}

		
		/* file open and read example */
		if ( file_fopen( &filer, &efs.myFs , "HELLO.TXT", 'r' ) == 0 ) 
		{	
			int eof = 0,i;
			
			xprintf("\n------ HELLO.TXT file ------\n");
			
			while(!eof) /* while file not finished */
			{	   
				res = file_read( &filer, 256, (unsigned char *)str ); /* read 256 bytes max */
				if (res <256) eof=1;
					
				i=0;
				
				while(res--)	/* print all bytes in buffer */
				{
					VCOM_putchar(str[i++]);
				}
			}
			
			xprintf("----------------------------\n");
			
			file_fclose(&filer);
			
		}
		else
		{
			xprintf("Error opening HELLO.TXT, file not found in microSD card\n");
		}
		
		fs_umount(&efs.myFs);  // Unmount filesystem, really important when you're writing files 
		                        // and want to flush everything to disk
		
		LEDS_off(LED1);
	}

}




