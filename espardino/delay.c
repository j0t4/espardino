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
static unsigned int delay_coef_ms;

static unsigned int last_t1tc_us;
static unsigned int last_t1tc_ms;

static unsigned int ovf_t1tc_ms;
static unsigned int ovf_t1tc_ms_inc;
static unsigned int ovf_t1tc_us;
static unsigned int ovf_t1tc_us_inc;

int delay_initialized = 0;

void delay_init()
{
		T1MCR = 0;
		T1PC  = 0; // No prescaling
		T1TCR = 1; //timer 1 enable

		delay_coef_us = VPB_get_speed()/1000000;
		delay_coef_ms = VPB_get_speed()/1000;

		last_t1tc_ms = 0;
		last_t1tc_us = 0;

		ovf_t1tc_ms = 0;
		ovf_t1tc_us = 0;

		ovf_t1tc_ms_inc = (0xffffffff)/delay_coef_ms;
		ovf_t1tc_us_inc = (0xffffffff)/delay_coef_us;

	  delay_initialized = 1;
}

void t1_init()
{
	delay_init();
}



unsigned int t1_get_ms()
{
	unsigned int c_T1TC;

  if (!delay_initialized) t1_init();

	c_T1TC = T1TC;
	if (c_T1TC<last_t1tc_ms)
	{
		ovf_t1tc_ms += ovf_t1tc_ms_inc;
	}

	last_t1tc_ms = c_T1TC;
	c_T1TC = (c_T1TC/delay_coef_ms) + ovf_t1tc_ms;

	return c_T1TC;
}

unsigned int t1_get_us()
{
	unsigned int c_T1TC;

	if (!delay_initialized) t1_init();
	
	c_T1TC = T1TC;

	if (c_T1TC<last_t1tc_us)
	{
		ovf_t1tc_us += ovf_t1tc_us_inc;
	}

	last_t1tc_us = c_T1TC;
	c_T1TC = (c_T1TC/delay_coef_us) + ovf_t1tc_us;
	return c_T1TC;

}






void delay_us (unsigned long usdelay)
{


	unsigned int c_T1TC;
	unsigned int next_T1TC;

	if (!delay_initialized) delay_init(); 

	c_T1TC= T1TC;
	next_T1TC = c_T1TC + usdelay*delay_coef_us;

	if (next_T1TC<c_T1TC) //overflow
	{
		while(T1TC&0xffffff00); // wait while all the upper bits are "ON"
	}

	while ((T1TC<next_T1TC) && (T1TC>c_T1TC) ); /* we assume that we'll do it on time */

}

void delay_ms ( unsigned long mdelay)
{
		
	delay_us(mdelay*1000);
}


void delay(float time)
{
	delay_us(time*1000000.0);
}
