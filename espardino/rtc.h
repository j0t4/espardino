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


#ifndef __RTC_H
#define __RTC_H

/*==FILE lib/rtc.txt */

/*==INTRO

Espardino's LPC214x has a Real Time Clock module to store the date/time information, it can 
be backed up during power off connecting a 3.3V battery to the VBAT input.

It defines the following structure to work with the RTC:

<code c>
typedef struct {
	unsigned short	year;	// Year
	unsigned char	month;	// Month 
	unsigned char	dom;	// Day of month
	unsigned char	dow;	// Day of week
	unsigned char	hour;	// Hour
	unsigned char	min;	// Minute
	unsigned char	sec;	// Second
} RTC;
</code>

<note>
[[lib:microsd::FFS]] module uses this functionality when you write to files,
storing the correct file time.
</note>

*/

/*==RESOURCES RTC module.  */


/*==USAGE Using the RTC in your applications
    
1st) Edit your makefile to include the **"micro214x"** library at linking time. You can modify the espardino base projects adding "-lmicro214x" to the MY_LIBS var.

<code c>
  # System libraries to be linked with
  MY_LIBS = -lmicro214x
</code>
	
2nd) Include the "micro214x.h" headers in your application.

<code c>
  #include <micro214x.h>
</code>

3rd) Call RTC_get(&rtc_info) or RTC_set(&rtc_info) as needed.

<code c>
  RTC rtc_info;        // define our RTC structure
  
  RTC_get(&rtc_info);  // Call RTC_get, with a pointer to our structure, it will fill it
 
  printf("RTC Time: %02d:%02d:%02d\r\n", // Finally, print the time! :-)
          rtc_info.hour, rtc_info.min, rtc_info.sec);

</code>

*/

/*==SECTION RTC library functions */



#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	unsigned short	year;	
	unsigned char	month;	
	unsigned char	dom;	
	unsigned char	dow;	
	unsigned char	hour;	
	unsigned char	min;	
	unsigned char	sec;	
} RTC;


/*==FUN void RTC_get (RTC *rtc_info) ===================================================
==ACTION
Reads the RTC information, and stores it in the rtc_info structure, you should pass it using a pointer to your RTC
structure variable. 
*/
void RTC_get (RTC*);	

/*==FUN void RTC_set (const RTC *rtc_info) ===================================================
==ACTION
Stores the RTC information from the rtc_info structure, you should pass it using a pointer to your RTC
structure variable. 
*/				
void RTC_set (const RTC*);				

#ifdef __cplusplus
}
#endif

#endif
