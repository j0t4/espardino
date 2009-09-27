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



#ifndef __SYSCLOCKS_H
#define __SYSCLOCKS_H

/* please refer to http://wiki.espardino.com/lib/system_clocks */

/*==FILE lib/system_clocks.txt */

/*==INTRO

System clocks library is intended to get and set information about our
main clock speed, and the peripherals bus speed (VPB). All library functions
ask this module to setup all the initialization that rely on system speed.

<note important>
If you change the PLL config, or the VPB speed divider you'll have to call
these functions before calling other modules initialization.
</note>

*/

/*==USAGE Accessing the system clocks info from your application

==INCLUDE wiki_micro214x_usage.txt

3rd) Call the desired functions.

<code c>
	int sys_speed_Hz = SYSCLK_get_speed();
	printf("Our system clock runs at %d Hz\r\n",sys_speed_Hz);
</code>


*/

/*==SECTION System clocks library funtions */



#ifdef __cplusplus
extern "C" {
#endif

/*==FUN void SYSCLK_set_speed(int speed_hz) ===================================================
==ACTION
Setups the library info about system clock speed, all the other libraries will ask 
for this value at initialization, so it's important to setup this value before calling
any other libraries _init function.
*/

void SYSCLK_set_speed(int speed_hz);


/*==FUN int SYSCLK_get_speed() ===================================================
==ACTION
Gets the current info about system clock speed, useful if we need to setup some hardware
register manually.
==RESULT
System clock speed in Hz.
*/
int SYSCLK_get_speed();

/*==FUN void VPB_set_speed(int speed_hz) ===================================================
==ACTION
Setups the library info about peripheral clock speed, all the other libraries will ask 
for this value at initialization, so it's important to setup this value before calling
any other libraries _init function.
*/
void VPB_set_speed(int speed_hz);

/*==FUN int VPB_get_speed() ===================================================
==ACTION
Gets the current info about peripheral clock speed, useful if we need to setup some hardware
register manually.
==RESULT
System clock speed in Hz.
*/
int VPB_get_speed();





#ifdef __cplusplus
}
#endif

#endif 

