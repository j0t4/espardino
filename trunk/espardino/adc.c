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


#include "adc.h"
#include "sysclocks.h"

#include <LPC214x.h>

int ADC_clkdiv_calc = -1;

void ADC_init (int mask)
{

  if (ADC_clkdiv_calc<0)
  {
	ADC_clkdiv_calc = (VPB_get_speed() / 4500000) -1;
	if (ADC_clkdiv_calc<1) ADC_clkdiv_calc=1;
  }
 
  if (mask&AD0_1)
	PINSEL1 = (PINSEL1 & ~(3<<24)) | (1<<24); // AD0.1 on P0.28
	
  if (mask&AD0_2)
	PINSEL1 = (PINSEL1 & ~(3<<26)) | (1<<26); // AD0.2 on P0.29 
	
  if (mask&AD0_3)
	PINSEL1 = (PINSEL1 & ~(3<<28)) | (1<<28); // AD0.3 on P0.30
  
  if (mask&AD0_4)
	PINSEL1 = (PINSEL1 & ~(3<<18)) | (1<<18); // AD0.4 on P0.25

  if (mask&AD0_6)
	PINSEL0 |= (3<<8); // AD0.6 on P0.4 
	
  if (mask&AD0_7)
	PINSEL0 |= (3<<10); // AD0.7 on P0.5



  if (mask&AD1_0)
	PINSEL0 |= (3<<12); // AD1.0 on P0.6  (LPC2144/46/48)

  if (mask&AD1_1)
	PINSEL0 |= (3<<16); // AD1.1 on P0.8  (LPC2144/46/48)
	
  if (mask&AD1_2)
	PINSEL0 |= (3<<20); // AD1.2 on P0.10  (LPC2144/46/48)
	
  if (mask&AD1_3)
	PINSEL0 |= (3<<24); // AD1.3 on P0.12  (LPC2144/46/48)
  
  if (mask&AD1_4)
	PINSEL0 |= (3<<26); // AD1.4 on P0.13  (LPC2144/46/48)

  if (mask&AD1_5)
	PINSEL0 |= (3<<30); // AD1.5 on P0.15  (LPC2144/46/48)
	
  if (mask&AD1_6)
	PINSEL1 = (PINSEL1 & ~(3<<10)) | (2<<10); // AD1.6 on P0.21  (LPC2144/46/48)

  if (mask&AD1_7)
	PINSEL1 = (PINSEL1 & ~(3<<12)) | (1<<12); // AD1.7 on P0.22  (LPC2144/46/48)

}

static int ADC_precission_data =0;
void ADC_set_precission(int bits)
{
	switch(bits)
	{
		case 10: ADC_precission_data= 0;
		case 9: ADC_precission_data = 1;
		case 8: ADC_precission_data = 2;
		case 7: ADC_precission_data = 3;
		case 6: ADC_precission_data = 4;
		case 5: ADC_precission_data = 5;
		case 4: ADC_precission_data = 6;
		case 3: ADC_precission_data = 7;
		
		default:
			if (bits<3) ADC_precission_data =7;
			else if (bits>10) ADC_precission_data=0;
	}
}

int ADC_read(int adc)
{
		ADC_start(adc);
		while (!ADC_done(adc));
		return ADC_get(adc);
}

void ADC_start(int adc)
{
	int clkdiv = ADC_clkdiv_calc; // (VPB_CLOCK / 4500000) -1   = 13 for 60Mhz , 7 for 30Mhz 
	
	if (adc<=AD0_7)  { /* ADC 0 */
	
		AD0CR   = (1<<24) |(1<<21) | (ADC_precission_data<<17) | (clkdiv<<8)|adc;     
		
	} else {	/* ADC 1 */
	
		adc=adc>>8;
	
		AD1CR   = (1<<24) |(1<<21) | (ADC_precission_data<<17) | (clkdiv<<8)|adc;     
		
	}

	
}

int ADC_done(int adc)
{
	
	if (adc<=AD0_7)  
	{
		return (AD0GDR&(1<<31));
	}
	else
	{
		return (AD1GDR&(1<<31));
	}
}

int ADC_get(int adc)
{
	int val;
	
	if (adc<=AD0_7)  {
		val = AD0GDR;
		AD0CR &= ~(1<<24); /* stop conversion */

		return (val>>6)&0x3ff;
	}
	else
	{
		val = AD1GDR;
		AD1CR &= ~(1<<24); /* stop conversion */

		return (val>>6)&0x3ff;
	}

}
