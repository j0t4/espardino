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



#include "leds.h"
#include "iolib.h"
#include <LPC214x.h>

void LEDS_init()
{
	/*IODIR1 |= LED3 | LED2 | LED1;
	IOSET1 = (LED1|LED2|LED3);*/
}

void LEDS_on(int mask)
{
/*	IOCLR1 = mask & (LED1|LED2|LED3); */
	if (mask&LED1) IO_output_low(P1_22);
	if (mask&LED2) IO_output_low(P1_21);
	if (mask&LED3) IO_output_low(P1_20);
}
void LEDS_off(int mask)
{
	/*IOSET1 = mask & (LED1|LED2|LED3);*/
	if (mask&LED1) IO_output_high(P1_22);
	if (mask&LED2) IO_output_high(P1_21);
	if (mask&LED3) IO_output_high(P1_20);
}

