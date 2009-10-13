/***************************************************************/
/* Copyright (c) 2009 NBEE Embedded Systems S.L.               */
/* All rights reserved.                                        */
/*                                                             */
/* Permission is hereby granted, free of charge, to any        */
/* person obtaining a copy of this software and associated     */
/* documentation files (the "Software"), to deal in the        */
/* Software without restriction, including without limitation  */
/* the rights to use, copy, modify, merge, publish,            */
/* distribute, sublicense, and/or sell copies of the           */
/* Software, and to permit persons to whom the Software is     */
/* furnished to do so, subject to the following conditions:    */
/*                                                             */
/* This code is distributed only with and for Espardino(tm)    */
/* products or derivative final products not intended as       */
/* development systems.                                        */
/*                                                             */
/* The above copyright notice and this permission notice       */
/* shall be included in all copies or substantial portions of  */
/* the Software.                                               */
/*                                                             */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY   */
/* KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE  */
/* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR     */
/* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS  */
/* OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR    */
/* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR  */
/* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE   */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.      */
/***************************************************************/


#include "miniprintf.h"
#include <math.h>

#ifndef NULL
#define NULL ((void *)0)
#endif


#define PADDING_TO_RIGHT (1<<1)
#define PADDING_WITH_ZEROS  (1<<0)
#define BUFFER_LEN 16


/* Function prototypes */
static void xprintf_putchar(char **s, int c);

static int xprintf_prints(char **out, const char *s, int width, int pad);

static int xprintf_print_integer(char **out, int i, int b, int sign, int width,
				 int pad, int base_2nd,int force_neg);

static int xprintf_print_float(char **out,double d, int width, int pad,
				int width_dec,int pad_dec);

int xprintf_print(char **out, int *varg);


/* Function implementation */

int xprintf(const char *format, ...)
{
	 int *varg = (int *)(&format);
	return xprintf_print(0, varg);
}

int xsprintf(char *out, const char *format, ...)
{
	 int *varg = (int *)(&format);
	return xprintf_print(&out, varg);
}

int xfprintf(printFuncPt out, const char *format, ...)
{
	 int *varg = (int *)(&format);
	return xprintf_print((char**)&out, varg);
}


static printFuncPt printFunc=NULL;

void xprintf_output(printFuncPt p)
{
	printFunc = p;
}


static void xprintf_putchar(char **s, int c)
{
	printFuncPt pf;


	if (s)
	{
	    if ((int)(*s)<0x10000000)  // is s in rom area?, then it should be a function
		{
			pf = (printFuncPt)*s;
			pf(c);
			return;
		}
		else
		{
			**s = c;
			(*s)++;
		}
	}
	else
	{
		if (printFunc!=NULL)
		{
			printFunc(c);
		}
	}
}


void xputc(int c)
{
	xprintf_putchar(NULL,c);
}

int xputs(const char *s)
{
	int count=0;
	if (printFunc!=NULL)
	{
		while (*s)
		{
			printFunc(*s++);
			count++;
		}
	}
	return count;
}



static int xprintf_prints(char **out, const char *s, int width, int pad)
{
	 int pad_count = 0;
	 int padchar = ' ';

	if (width > 0)
	{
		int len = 0;
		const char *ptr;

		for (ptr = s; *ptr; ptr++) len++;

		if (len >= width) width = 0;
		else 		  width -= len;

		if (pad & PADDING_WITH_ZEROS) padchar = '0';
	}
	if (!(pad & PADDING_TO_RIGHT)) {
		while (width > 0) {
			xprintf_putchar (out, padchar);
			pad_count++;
			width--;
		}
	}
	while (*s) {
		xprintf_putchar (out, *s++);
		pad_count++;
	}
	while (width > 0) {
		xprintf_putchar (out, padchar);
		width--;
		pad_count++;
	}

	return pad_count;
}


static int xprintf_print_integer(char **out, int i, int b, int sign, int width, int pad, int base_2nd,int force_neg)
{
	char print_buf[BUFFER_LEN];
	 char *s;
	 int t, neg = 0, pad_count = 0;
	 unsigned int u = i;

	if (i == 0) {
		t= 0;
		if (force_neg) {
			print_buf[t++]='-';
		}
		print_buf[t++] = '0';
		print_buf[t++] = '\0';
		return xprintf_prints (out, print_buf, width, pad);
	}

	if (sign && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + BUFFER_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += base_2nd - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg|force_neg) {
		if( width && (pad & PADDING_WITH_ZEROS) ) {
			xprintf_putchar (out, '-');
			pad_count++;
			width--;
		}
		else {
			*--s = '-';
		}
	}

	return pad_count + xprintf_prints (out, s, width, pad);
}

static int xprintf_print_float(char **out,double d, int width, int pad, int width_dec,int pad_dec)
{

	int pad_count = 0;

	int neg = (d<0.0f);

	pad_count += xprintf_print_integer (out, (int)d, 10, 1, width, pad, 'a' ,neg);

	if (neg) d=-d;

	d -= (int)floor(d);

	if ((!(pad_dec|PADDING_WITH_ZEROS)) && (fabs(d)<0.00000000000001))
	{
		return pad_count;
	}
	else
	{
		if (width_dec==0) width_dec = 8;
		xprintf_putchar(out,'.');
		while(width_dec--)
		{
		  d = d* (double)10.0f;
		  xprintf_putchar(out,'0' +((int)floor(d)));
		  d -= (int)floor(d);
		}

	}

	return pad_count;

}


int xprintf_print(char **out, int *varg)
{
	int width, width_dec, pad,pad_dec;
	char char_buffer[2];
	int pad_count = 0;
	char *format = (char *)(*varg++);

	while (*format) {
		if ((format[0] == '%') && (format[1]!='%')) {
			format++;
			width = 0;
			pad = 0;
			width_dec = 0;
			pad_dec = 0;
			if (*format == '\0') break;
			if (*format == '-') {
				format++;
				pad = PADDING_TO_RIGHT;
			}
			while (*format == '0') {
				format++;
				pad |= PADDING_WITH_ZEROS;
			}
			for ( ; *format >= '0' && *format <= '9'; format++) {
				width *= 10;
				width += *format - '0';
			}
			if( *format=='.')
			{
				format++;

				while (*format == '0') {
				format++;
				pad_dec |= PADDING_WITH_ZEROS;
				}

				for ( ; *format >= '0' && *format <= '9'; format++) {
					width_dec *= 10;
					width_dec += *format - '0';
				}
			}
			if( *format == 'x' )
			{
				pad_count += xprintf_print_integer (out, *varg++, 16, 0, width, pad, 'a',0);
				format++;
				continue;
			}
			if( *format == 'X' ) {
				pad_count += xprintf_print_integer (out, *varg++, 16, 0, width, pad, 'A',0);
				format++;
				continue;
			}
			if (*format=='g')
			{
				double d = *((double *)varg);
				varg+=sizeof(double)/sizeof(int);
				pad_count += xprintf_print_float (out,d,width,pad,width_dec,pad_dec);
				format++;
				continue;
			}
			if( *format == 's' )
			{
				 char *s = *((char **)varg++);
				pad_count += xprintf_prints (out, s?s:"(null)", width, pad);
				format++;
				continue;
			}

			if( *format == 'd' )
			{
				pad_count += xprintf_print_integer (out, *varg++, 10, 1, width, pad, 'a',0);
				format++;
				continue;
			}
			if( *format == 'p')
			{
				pad_count += xprintf_print_integer (out,*varg++, 16,  0, width, pad, 'a',0);
				format++;
				continue;
			}
			if( *format == 'u' )
			{
				pad_count += xprintf_print_integer (out, *varg++, 10, 0, width, pad, 'a',0);
				format++;
				continue;
			}
			if( *format == 'c' )
			{
				char_buffer[0] = *varg++;
				char_buffer[1] = '\0';
				pad_count += xprintf_prints (out, char_buffer, width, pad);
				format++;
				continue;
			}
		}
		else
		{
			if((format[0] == '%') && (format[1]=='%')) format++;
			xprintf_putchar (out, *format);
			pad_count++;
		}

		format++;
	}

	if (out &&((int)(*out)>0x10000000)  ) **out = '\0';  // If it's a s (and not rom function), terminate ASCIIZ
	return pad_count;
}



