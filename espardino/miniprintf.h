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



#ifndef __MINIPRINTF_H
#define __MINIPRINTF_H

/*==FILE lib/printf.txt */

/*==INTRO

Espardino libraries come with a mini printf functionality, allowing you to do
basic printf / sprintf, without the overhead of all the standard library 
that comes with the big "printf".

Supported modes are: %d %u %x %X %c %p %g and %s  with all the variants for padding,
and length setup.

*note*: The %g (tiny float support) it's still experimental. Please use with caution.

*/


/*==USAGE Using the mini printf implementation

==INCLUDE wiki_micro214x_usage.txt

3rd) If you want to use printf, first call xprintf_output once specifying a 
pointer your character output function. To use sprintf you don't need
to do that.

<code c>
xprintf_output(&VCOM_putchar_nl);
</code>

4rd) Use the printfs 

<code c>
xprintf("This is a test: %d, %08x\n",1024,0x12344ffe); // this will print 1024, 12344ffe
xprintf("Now with padding %04d\n",12);                 // this will print 0012
</code>

*/

/*==SECTION mini-printf library functions */

#ifdef __cplusplus
extern "C" {
#endif
	typedef int(*printFuncPt)(int);
	
/*==FUN void xprintf_output(printFuncPt p) ===================================================
==ACTION
Setups the character output function for printf and puts.
*/	
        void xprintf_output(printFuncPt p);
	
/*==FUN int xputs(const char *str) ===================================================
==ACTION
Prints a string out using the character output function setup with the previous function.
==RESULT
total number of printed characters.
*/		
    int xputs(const char *str);
	
/*==FUN int xputc(int c) ===================================================
==ACTION
Prints a character out using the character output function setup with the xprintf_output function
==RESULT
total number of printed characters.
*/			
	void xputc(int c);
	
/*==FUN int xprintf(const char *format, ...) ===================================================
==ACTION
Prints a formatted string with the parameters, using the character output function setup with the printf_output function.
==RESULT
total number of printed characters.
*/		
        int xprintf(const char *format, ...);

/*==FUN int xsprintf(char *str_out,const char *format, ...) ===================================================
==ACTION
Writes a formatted string with the parameters to a strinf buffer str_out. Use with care (don't overflow the output buffer).
==RESULT
total number of written characters.
*/		
	
        int xsprintf(char *out, const char *format, ...);
		

/*==FUN int xfprintf(printFuncPt,const char *format, ...) ===================================================
==ACTION
Writes a formatted string with the parameters to a character out function. The function should be in ROM area.
==RESULT
total number of written characters.
*/				
		int xfprintf(printFuncPt out, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
