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
#include "rtc.h"

void RTC_get (RTC *rtc)
{
	unsigned int ctime_0_copy,
			     ctime_1_copy;


	do {
		ctime_0_copy = CTIME0;
		ctime_1_copy = CTIME1;
	} while (ctime_0_copy != CTIME0 || ctime_1_copy != CTIME1);

	rtc->sec 	= ctime_0_copy & 0x3F;
	rtc->min 	= (ctime_0_copy >> 8) & 0x3F;
	rtc->hour 	= (ctime_0_copy >> 16) & 0x1F;
	rtc->dow 	= (ctime_0_copy >> 24) & 0x07;
	rtc->dom 	= ctime_1_copy & 0x1F;
	rtc->month 	= (ctime_1_copy >> 8) & 0x0F;
	rtc->year	 = (ctime_1_copy >> 16) & 0xFFF;

}




void RTC_set (const RTC *rtc)
{
	
	CCR = 0x12;  /* stop rtc */

	SEC = rtc->sec;
	MIN = rtc->min;
	HOUR = rtc->hour;
	DOW = rtc->dow;
	DOM = rtc->dom;
	MONTH = rtc->month;
	YEAR = rtc->year;

	CCR = 0x11; /* run rtc */

}


