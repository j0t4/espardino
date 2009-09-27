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


#ifndef __LEDS_H
#define __LEDS_H

#define LED3 (1<<20)
#define LED2 (1<<21)
#define LED1 (1<<22)

/*==FILE lib/leds.txt */

/*==INTRO

Espardino's micro214x come with 3 leds on board. They are connected to P1_22, P1_21, P1_20.
They can be hardware disabled removing the jumper at "LEDEN" uder RP1.

Leds are defined with the following mask macros: **LED1, LED2, LED3**

*/



/*==RESOURCES Digital IO ports: P1.22, P1.21, P1.20 */


/*==USAGE Using the led library in your applications
    
1st) Edit your makefile to include the **"micro214x"** library at linking time. You can modify the espardino base projects adding "-lmicro214x" to the MY_LIBS var.

<code c>
  # System libraries to be linked with
  MY_LIBS = -lmicro214x
</code>
	
2nd) Include the "micro214x.h" headers in your application.

<code c>
  #include <micro214x.h>
</code>

3rd) Call the LEDS_on(mask) / LEDS_off (mask) functions.
<code c>
  LEDS_on(LED1|LED2);  // switch on LED1 and LED2
  LEDS_off(LED3);      // switch off LED3
</code>

*/


#ifdef __cplusplus
extern "C" {
#endif

void LEDS_init();

/*==FUN void void LEDS_on(int mask) ===================================================
==ACTION
Switches on the led mask. Mask can be, for example: LED1  or  LED1|LED2, etc...

*/
void LEDS_on(int mask);

/*==FUN void void LEDS_off(int mask) ===================================================
==ACTION
Switches off the led mask. Mask can be, for example: LED1  or  LED1|LED2, etc...

*/
void LEDS_off(int mask);


#ifdef __cplusplus
}
#endif

#endif

