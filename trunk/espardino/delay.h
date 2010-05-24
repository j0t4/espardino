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


#ifndef __DELAY_H
#define __DELAY_H


#ifdef __cplusplus
extern "C" {
#endif

void delay_init(void);
void delay_us (unsigned long usdelay);
void delay_ms ( unsigned long mdelay);

void delay(float time);

#define wait(x) delay(x)

void t1_init(void);
unsigned int t1_get_us(void);
unsigned int t1_get_ms(void);


#ifdef __cplusplus
}
#endif

#endif


/*==FILE lib/delay.txt */

/*==INTRO

The delay library can be used to stop your program execution with microsecond
granularity. This can be useful when dealing with digital communication
protocols at I/O port level (I2C/SPI/one wire..), also for blinking
leds or any kind of timed control.

Reliable delay is managed thanks to Timer1 control, which makes it more/less
independent from IRQ execution. It should only be shifted if by the time
of finishing an IRQ/FIQ is on execution.

Timer1 can be readen using t1_get_ms() and t1_get_us() in millisecond/microsecond
granularity, these functions manage the T1 overflow (every 143,165567 seconds by
default) whenever they are called in a periodic maneer with period less than those
143 seconds).

An init function for t1_get_xx is included, it's t1_init(), it should be called at
program start to use the t1_get_ms or t1_get_us functions.

*/



/*==RESOURCES TIMER1 (also used by PWM_IN library), both can share it without problems,
it doesn't modify T1 count, just reads*/


/*==USAGE Using the delay library in your applications

1st) Edit your makefile to include the **"micro214x"** library at linking time.
You can modify the espardino base projects adding "-lmicro214x" to the MY_LIBS var.

<code c>
  # System libraries to be linked with
  MY_LIBS = -lmicro214x -lm
</code>

2nd) Include the "micro214x.h" headers in your application.

<code c>
  #include <micro214x.h>
</code>

3rd) Call the delay_init() function during your program startup

4th) Call the delay_us(x) or delay_ms(x) functions to stop your program
execution for a given time.
<code c>

    delay_init();

    LEDS_on(LED3);        // switch on LED3
    delay_ms(500);        // wait 500 milliseconds
    LEDS_off(LED3);      // switch off LED3
</code>

*/

