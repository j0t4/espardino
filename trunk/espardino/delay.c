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


#include <LPC214x.h>
#include "delay.h"
#include "sysclocks.h"

static unsigned int delay_coef_us;
void delay_init()
{
		T1MCR = 0;
		T1PC  = 0; // No prescaling
		T1TCR = 1; //timer 1 enable
		delay_coef_us = VPB_get_speed()/1000000;
}


	
void delay_us (unsigned long usdelay)
{

	
	unsigned int c_T1TC;
	unsigned int next_T1TC;
	
	
	c_T1TC= T1TC;
	next_T1TC = c_T1TC + usdelay*delay_coef_us; 
	
	if (next_T1TC<c_T1TC) //overflow
	{
		while(T1TC&0xffffff00); // wait while all the upper bits are "ON"
	}
	
	while (T1TC<next_T1TC); /* we assume that we'll do it on time */
	
}

void delay_ms ( unsigned long mdelay)
{

	delay_us(mdelay*1000);
}
