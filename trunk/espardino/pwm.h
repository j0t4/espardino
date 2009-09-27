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



#ifndef __PWM_H
#define __PWM_H

#define PWM_1 (1<<0)
#define PWM_2 (1<<1)
#define PWM_3 (1<<2)
#define PWM_4 (1<<3)
#define PWM_5 (1<<4)
#define PWM_6 (1<<5)

/*==FILE lib/pwm.txt */

/*==INTRO

PWM output library allows you to use up to 6 hardware PWM outputs. PWM_1 .. PWM_6.
PWM outputs are useful to control the position of servos, or motor speed with the 
help of mosfets / h bridges.

They can be used to output analog signals too, with the use of an R/C filter at pin output.

*/

/*==RESOURCES Shared with ports

^ Output  ^ Port  ^ Shared with 	^
| PWM_1   | P0.0  | TXD0			|
| PWM_2   | P0.7  | SSEL0 (microSD)	|
| PWM_3   | P0.1  | RXD0			|
| PWM_4   | P0.8  | TXD1			|
| PWM_5   | P0.21 | AD1.6			|
| PWM_6   | P0.9  | RXD1			|

*/


/*==USAGE Using the PWM output in your applications

==INCLUDE wiki_micro214x_usage.txt

3rd) Call PWM_init(mask) during your app initialization. This will setup the PWM pins in
PWM output mode.
<code c>
PWM_init(PWM_1|PWM_4); // P0.0 and P0.8 as PWM outputs
</code>

4rd) Call PWM_frequency(Hz_freq) with the Hz frequency you want the PWM output to work.

<code c>
PWM_frequency(50); // 50Hz / 20ms / control de servos 
</code>

*/

/*==SECTION PWM output library functions */


#ifdef __cplusplus
extern "C" {
#endif

/*==FUN void PWM_init (int mask) ===================================================
==ACTION
Initializes the selected pins as PWM output. Mask should be an or'd value with all the PWM
pins: PWM_1|PWM_2|PWM_4
*/
void PWM_init (int mask);

/*==FUN int PWM_frequency (int frequency) ===================================================
==ACTION
Setups the PWM frequency in Hertzs ( rising edge, to the next rising edge). This frequency affects
to all the PWM outputs.
*/
void  PWM_frequency (int frequency);


/*==FUN int PWM_dutycycle (int mask, int percentage) ===================================================
==ACTION
Setups the PWM duty cycle for a selected mask of PWM outputs. (The duty cycle is the time the
PWM will be up during the frequency. A 100% means all the time up, a 0% means all the time down. 50% means
half time up, half time down.
*/
void  PWM_dutycycle (int mask, int percentage);


/*==FUN int PWM_length(int mask, unsigned int count) ===================================================
==ACTION
This function is like PWM_dutycycle, but more fine-grained. Count can go from 0 to (VPB_get_speed() / frequency),
anyway this could change in the next library releases, use it with caution.
==EXTRA
<note important>
This function behaviour could change in next library releases, use it with caution.
</note>
*/
void  PWM_length(int mask, unsigned int count);

/*==FUN int PWM_pulsewidth_us (int mask, int us) ===================================================
==ACTION
Setups the PWM "up" time width for a selected mask of PWM outputs. This is the time the pulse will be high 
during frequency.

*/
void  PWM_pulsewidth_us (int mask, int us);




#ifdef __cplusplus
}
#endif

#endif
