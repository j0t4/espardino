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


#ifndef __ADC_H
#define __ADC_H

#define AD0_1 (1<<1)
#define AD0_2 (1<<2)
#define AD0_3 (1<<3)
#define AD0_4 (1<<4)
#define AD0_6 (1<<6)
#define AD0_7 (1<<7)

#define AD1_0 (1<<8)
#define AD1_1 (1<<9)
#define AD1_2 (1<<10)
#define AD1_3 (1<<11)
#define AD1_4 (1<<12)
#define AD1_5 (1<<13)
#define AD1_6 (1<<14)
#define AD1_7 (1<<15)


#ifdef __cplusplus
extern "C" {
#endif

void ADC_init (int mask); /* init a subset of ADC channels on the chip */
void ADC_set_precission(int bits); /* setup precission between 3 and 10 bits */
int ADC_read(int adc);  /* starts, waits, and gets the ADC result t=2.5 uS */

void ADC_start(int adc);	/* starts an ADC read on a channel */
int ADC_done(int adc); /* returns !=0 if channel finished reading */
int ADC_get(int adc); /* gets the result from a channel */


#ifdef __cplusplus
}
#endif

/*==FILE lib/adc.txt */
/*==INTRO 
**LPC2142** has 6 10bits analog input ports: **AD0.1,AD0.2, AD0.3 ,AD0.4, AD0.6, AD0.7**.

**LPC2148** has 8 10bit analog input ports in addition to AD0.1,2,3,4,6,7. They are **AD1.0, AD1.1, AD1.2, AD1.3,
AD1.4, AD1.5, AD1.6, AD1.7**.

All analog ports all are multiplexed with other pin functions, and their input is always 10bit relative to
 the VREF input pin. That means that if VREF is 2.8V and AD0.1 is 2.8V or greater a reading of AD0.1 should
 result in '1023' (that's ten bits as '1').

Espardino's ADC library is a fast and easy way to access the ADC inputs. It should be useful on most cases.
Greater speed can be get using IRQs or 'bulk' ADC reads.

<note important>
Remember to setup reference voltage on VREF pin if you want to use the ADC module. 
</note>

*/

/*==RESOURCES Shared with ports

^ ADC Input	^ Port	^ Functions			^   ^ADC Input		^ Port	^ Functions			^
| 	AD0.1 	| P0.28	| CAP0.3,MAT0.3		|   |	//AD1.0// 	| P0.6 	| MOSI0,CAP0.2		|
| 	AD0.2 	| P0.29	| CAP0.3,MAT0.3		|	|	//AD1.1// 	| P0.8  | TXD1,PWM4			|
| 	AD0.3 	| P0.30	| EINT3,CAP0.0		|	|	//AD1.2// 	| P0.10 | RTS1,CAP1.0		|
| 	AD0.4 	| P0.25 | AOUT					|	|	//AD1.3// 	| P0.12 | DSR1,MAT1.0		|
| 	AD0.6 	| P0.4 	| SCK0,CAP0.1  		|	|	//AD1.4// 	| P0.13 | DTR1,MAT1.1		|
|	AD0.7	| P0.5	| MISO0,MAT0.1      |	|	//AD1.5//	| P0.15 | RI1,EINT2			|
|	---		|		|					|	|	//AD1.6//	| P0.21 | PWM5,CAP1.3		|
|	---		|		|					|	|	//AD1.7//	| P0.22 | CAP0.0,MAT0.0		|







*/

/*==USAGE Using the Analog Input Ports

1st) Edit your makefile to include the 'micro214x' library at linking time. You can modify the espardino base
projects adding **'-lmicro214x'** to the MY_LIBS var.

<code c>
  # System libraries to be linked with
  MY_LIBS = -lmicro214x
</code>

2nd) Include the 'micro214x.h' headers in your application.

<code c>
  #include <micro214x.h>
</code>

3rd) Call ADC_init(mask) inside your application, during startup.It will setup all the selected pins as 
analog inputs.

<code c>
  ADC_init(AD0_1|AD0_2); // enable pins as analog input 
</code>

Available masks are: AD0_1, AD0_2, AD0_3, AD0_4, AD0_6, AD0_7, AD1_0, AD1_2, AD1_3, AD1_4, AD1_5, AD1_6, AD1_7.


*/

/*==SECTION Reading data from the ADC Ports */

/*==FUN void ADC_init (int mask) =============================================================
==ACTION
  Initializes the pins in mask as ADC input pin.
  
*/

/*==FUN void ADC_set_precission(int bits) ====================================================
==ACTION
It setups the ADC read precision in bits: from 3  to 10. Reducing precision we get a faster read, 
increasing precision we get more information, but we decrease the read speed.

*/


/*==FUN int ADC_read(int adc) =================================================================
==ACTION
Reads from an ADC channel, adc can be {AD0_1 .. AD1_7}. It will wait until ADC read is ready, 
then it will return the reading value.

==RESULT
Returns the analog value at ADC pin, within the configured precission
*/ 
 


/*==FUN void ADC_start(int adc) ===============================================================
==ACTION
It starts an ADC read on some channel {AD0_1 .. AD1_7}. It will return immediately.
*/

/*==FUN int ADC_done(int adc) =================================================================
==ACTION
It returns the completion status of a previously started ADC read. It will return 0 while the ADC read is still unfinished.

==RESULT
0 while not finished. !=0 when done.

*/

/*==FUN int ADC_get(int adc) ===================================================================
==ACTION
It will get the current reading of an ADC channel. It should be called after ADC_done returns something different than 0.

==RESULT
Returns the analog value at ADC pin, within the configured precission

*/
#endif

