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


#ifndef __IOLIB_H
#define __IOLIB_H

/*==FILE lib/io.txt */

/*==INTRO

Espardino's LPC214x has 2 32bit ports, that can be used as digital Input/Output pins. 
They are shared with other functions, check the [[starting::QuickGuide]] for more information.
	
They can work in "SLOW" (legacy mode, for compatibility with old code you can
find on the internet), or "FAST". You should setup the desired mode using IO_init(FAST_IO) 
/ IO_init(SLOW_IO) at start inside main.
	
<note important>
Default mode is "slow" for compatibility reasons, if you want to use the fast
mode with FIOx registers you should call IO_init at start of main.
</note>
	
	

*/



/*==RESOURCES Ports used as IO  */


/*==USAGE Using the input/output library in your applications
    
1st) Edit your makefile to include the **"micro214x"** library at linking time. You can modify the espardino base projects adding "-lmicro214x" to the MY_LIBS var.

<code c>
  # System libraries to be linked with
  MY_LIBS = -lmicro214x
</code>
	
2nd) Include the "micro214x.h" headers in your application.

<code c>
  #include <micro214x.h>
</code>

3rd) Call IO_init(mode) inside your application, during startup.It will setup the IO/mode. Right now that's only needed
if you want to setup I/O ports in FAST_IO mode. SLOW_IO is the default mode.

<code c>
  IO_init(FAST_IO);	// init the ports as FAST IO
</code>

*/

/*==SECTION I/O library functions
All I/O functions that can receive a pin as parameter should do it with the P0_0, P0_1, P0_xx or P1_0, P1_1, P1_xx ... macros.

*/


#ifdef __cplusplus
extern "C" {
#endif


/*==FUN void IO_init (int mode) ===================================================
==ACTION
Setups the I/O mode, mode can be FAST_IO or SLOW_IO, it's important to call this init 
at main() start, all other modules will rely on this initialization.

*/
void IO_init (int mode);
 
 
 
/*==FUN int IO_input(int pin) ===================================================
==ACTION
Setups one //pin// as input, and returns it's digital value (true/false)
==RESULT
0 if pin value is '0', !=0 if value is '1'
*/
int IO_input(int pin);



/*==FUN void IO_output(int pin, int value) ===================================================
==ACTION
Setups one //pin// as output, and outputs the value (logical '0' or '1')

*/
void IO_output(int pin, int value);



/*==FUN void IO_output_low(int pin) ===================================================
==ACTION
setups one //pin// as output, and outputs a '0' value
*/

void IO_output_low(int pin);


/*==FUN void IO_output_high(int pin) ===================================================
==ACTION
setups one //pin// as output, and outputs a '1' value

*/
void IO_output_high(int pin);





/*==FUN int  IO_get_mode() ===================================================
==ACTION
This is used to ask the system it's current IO mode, so you can use the registers
directly for faster Input/Output. Check the [[advanced:Advanced IO Guide]]. 

==RESULT
FAST_IO or SLOW_IO

==EXTRA

<note>
If you need the  maximum speed you can access those ports directly using the processor registers 
//FIOxDIR, IOxDIR, FIOxSET, IOxSET, FIOxCLR,IOxCLR, FIOxPIN, IOxPIN//, check the [[advanced:Advanced IO Guide]].
</note>

*/
int  IO_get_mode();  /* returns FAST_IO or SLOW_IO */

#ifdef __cplusplus
}
#endif


/*==SECTION Example

Wait until FLASH_EN button is pressed, then wait until it's released
<code c>
   
   while (IO_input(P1_16)!=0); // Wait for P1_16 to go low (pushed)
   
   while (IO_input(P1_16)==0); // Wait for P1_16 to be released
   
</code>

*/


#define PORT_P0 (0<<8)
#define PORT_P1 (1<<8)

#define PORT_MASK (255<<8)

#define FAST_IO   1
#define SLOW_IO   0

#define HIGH 1
#define LOW 0

#define DEFAULT_IO FAST_IO



#define P0_0 	(0|PORT_P0)
#define P0_1 	(1|PORT_P0)
#define P0_2 	(2|PORT_P0)
#define P0_3 	(3|PORT_P0)
#define P0_4 	(4|PORT_P0)
#define P0_5 	(5|PORT_P0)
#define P0_6 	(6|PORT_P0)
#define P0_7 	(7|PORT_P0)
#define P0_8 	(8|PORT_P0)
#define P0_9 	(9|PORT_P0)
#define P0_10	(10|PORT_P0)
#define P0_11	(11|PORT_P0)
#define P0_12   (12|PORT_P0)
#define P0_13	(13|PORT_P0)
#define P0_14	(14|PORT_P0)
#define P0_15	(15|PORT_P0)
#define P0_16	(16|PORT_P0)
#define P0_17	(17|PORT_P0)
#define P0_18	(18|PORT_P0)
#define P0_19	(19|PORT_P0)
#define P0_20	(20|PORT_P0)
#define P0_21	(21|PORT_P0)
#define P0_22	(22|PORT_P0)
#define P0_23	(23|PORT_P0)
#define P0_24	(24|PORT_P0)
#define P0_25	(25|PORT_P0)
#define P0_26	(26|PORT_P0)
#define P0_27	(27|PORT_P0)
#define P0_28	(28|PORT_P0)
#define P0_29 	(29|PORT_P0)
#define P0_30 	(30|PORT_P0)
#define P0_31 	(31|PORT_P0)

#define P1_0 	(0|PORT_P1)
#define P1_1 	(1|PORT_P1)
#define P1_2 	(2|PORT_P1)
#define P1_3 	(3|PORT_P1)
#define P1_4 	(4|PORT_P1)
#define P1_5 	(5|PORT_P1)
#define P1_6 	(6|PORT_P1)
#define P1_7 	(7|PORT_P1)
#define P1_8 	(8|PORT_P1)
#define P1_9 	(9|PORT_P1)
#define P1_10	(10|PORT_P1)
#define P1_11	(11|PORT_P1)
#define P1_12   (12|PORT_P1)
#define P1_13	(13|PORT_P1)
#define P1_14	(14|PORT_P1)
#define P1_15	(15|PORT_P1)
#define P1_16	(16|PORT_P1)
#define P1_17	(17|PORT_P1)
#define P1_18	(18|PORT_P1)
#define P1_19	(19|PORT_P1)
#define P1_20	(20|PORT_P1)
#define P1_21	(21|PORT_P1)
#define P1_22	(22|PORT_P1)
#define P1_23	(23|PORT_P1)
#define P1_24	(24|PORT_P1)
#define P1_25	(25|PORT_P1)
#define P1_26	(26|PORT_P1)
#define P1_27	(27|PORT_P1)
#define P1_28	(28|PORT_P1)
#define P1_29 	(29|PORT_P1)
#define P1_30 	(30|PORT_P1)
#define P1_31 	(31|PORT_P1)










#endif

