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


#include "sysclocks.h"
#include <LPC214x.h>
#include "spi.h"

// SSPCR0  Bit-Definitions
#define SPI1_CPOL    6
#define SPI1_CPHA    7
// SSPCR1  Bit-Defintions
#define SPI1_SSE     1
#define SPI1_MS      2
#define SPI1_SCR     8
// SSPSR  Bit-Definitions
#define SPI1_TNF     1
#define SPI1_RNE     2
#define SPI1_BSY		4


#define SPI0_CPHA    3
#define SPI0_CPOL    4
#define SPI0_MSTR    5
// SP0SPSR  Bit-Definitions
#define SPI0_SPIF    7

#define SPI0_DIVIDER 8



void SPI_init (int channel_mask, int bits, int bps)
{
	(void) bits;
	int dummy, i;
	int div_calc;
	if (channel_mask&SPI0)
	{
		PINSEL0 &= ~(
		    		    (3UL << 8) | // SCK
		                (3UL << 10) | // MISO
		                (3UL << 12) | // MOSI
		                (3UL << 14)   // SSEL
		                                );
		        // set pin-functions to SSP
		        PINSEL0 |=
		                (1UL << 8) | // SCK
		                (1UL << 10) | // MISO
		                (1UL << 12) ;  // MOSI
		                // slave-select handled though GPIO

		        S0SPCR = (1<<SPI0_MSTR)|(0<<SPI0_CPOL);   /* enable as master, set polarity */

						div_calc = ((VPB_get_speed()/bps)+1)&0xFFFFFFFE; /* last bit must be always 0 */
						if (div_calc<8) div_calc=8;	/* cannot be <8, or the SPI channel won't work */
							
						S0SPCCR = div_calc;
	}
	
	if (channel_mask&SPI1)
	{
		
		SSPCR1 &= ~(1<<SPI1_SSE);

		SSPCR0 = ((8-1)<<0) | (0<<SPI1_CPOL) | (0x2<<SPI1_SCR); //  (0xff<<SCR);  // 8 bit transfers

		PINSEL1 = (PINSEL1 & (~(3<<2)))|(2<<2); // SCK1
		PINSEL1 = (PINSEL1 & (~(3<<4)))|(2<<4); // MISO1
		PINSEL1 = (PINSEL1 & (~(3<<6)))|(2<<6); // MOSI1

		div_calc = (VPB_get_speed()/bps); 
		if (div_calc<2) div_calc=2;	/* cannot be <2, or the SPI channel won't work */
							
		SSPCPSR = div_calc; //2, 4 , 5
		SSPCR1 = (1<<SPI1_SSE); // Enable SSP, as Master
		
		// clean up the fifo
		for (i=0;i<8;i++)
		{
			dummy = SSPDR;
		}
	}
	
}


void SPI_wait(int channel_mask)
{
		if (channel_mask&SPI0)
			 while( !(S0SPSR & (1<<SPI0_SPIF)) ) ;
		
		
		if (channel_mask&SPI1)
			while (SSPSR & (1<<SPI1_BSY));
	
}

unsigned int SPI_xfer (int channel, unsigned int data)
{
		unsigned int rxData=0;
	
		
		
		if (channel&SPI0)
		{
			S0SPDR = data;
			SPI_wait(channel);
			rxData = S0SPDR;
		}
		
		if (channel&SPI1)
		{
			SSPDR = data;
			while (SSPSR & (1<<SPI1_BSY));
			rxData  = SSPDR;
		}
		
		return rxData;
}

