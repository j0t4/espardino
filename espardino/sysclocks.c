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

int SYSCLK_speed_Hz = 60000000;

void VPB_set_speed(int speed_hz)
{
	int sysclk_d1 = SYSCLK_speed_Hz;
	int sysclk_d4 = SYSCLK_speed_Hz/4;
	int sysclk_d2 = SYSCLK_speed_Hz/2;

	sysclk_d1-=speed_hz;
	if (sysclk_d1<0) sysclk_d1=-sysclk_d1;

	sysclk_d2-=speed_hz;
	if (sysclk_d2<0) sysclk_d2=-sysclk_d2;

	sysclk_d4-=speed_hz;
	if (sysclk_d4<0) sysclk_d4=-sysclk_d4;

	/* check differences from the given hz */

	if      (sysclk_d1<sysclk_d2) VPBDIV=1;  /* div by 1 */
	else if (sysclk_d2<sysclk_d4) VPBDIV=2; /* div by 2 */
	else				   		  VPBDIV=4; /* div by 4 */
}

int VPB_get_speed()
{


	switch(VPBDIV&0x3)
	{
		case 0: return SYSCLK_speed_Hz/4;
		case 1: return SYSCLK_speed_Hz;
		case 2: return SYSCLK_speed_Hz/2;
		default: return SYSCLK_speed_Hz; /*reserved, shouldn't happen */

	}

}


void SYSCLK_set_speed(int speed_hz)
{
	SYSCLK_speed_Hz = speed_hz;
}

int SYSCLK_get_speed()
{
    return SYSCLK_speed_Hz;
}
