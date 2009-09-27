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


#include <iolib.h>
#include <LPC214x.h>


static int IO_mode = SLOW_IO;

void IO_init (int mode)
{
	IO_mode = mode & 1;
	
	if (IO_mode == FAST_IO)
	{
		SCS = (1<<1) | (1<<0);  // P0 and P1 as fast IO
	}
	else
	{
		SCS = 0;
	}
}
 
int IO_input(int pin_mask)
{
	unsigned int port;
	unsigned int pin;
	unsigned int mask;
	
	pin = pin_mask & (~PORT_MASK);
	port =pin_mask & (PORT_MASK);
	
	mask = (1<<pin);
	
	if (IO_mode == FAST_IO)
	{
		switch (port)
		{
			case PORT_P0: FIO0DIR &= ~mask;
						  return (FIO0PIN & mask)?1:0;
			case PORT_P1: FIO1DIR &= ~mask;
			              return (FIO1PIN & mask)?1:0;
		}
	}  	
	else
	{
		switch (port)
		{
			case PORT_P0: IO0DIR &= ~mask;
						  return (IO0PIN & mask)?1:0;
						  
			case PORT_P1: IO1DIR &= ~mask;
						  return (IO1PIN & mask)?1:0;
		}
	}	
	return -1;
}

void IO_output(int pin_mask, int value)
{
	unsigned int port;
	unsigned int pin;
	unsigned int mask;
	
	pin = pin_mask & (~PORT_MASK);
	port =pin_mask & (PORT_MASK);
	
	mask = (1<<pin);
	
	if (IO_mode == FAST_IO)
	{
		switch (port)
		{
			case PORT_P0: FIO0DIR |= mask;
						  if (value) FIO0SET = mask;
						  else       FIO0CLR = mask;
						  break;
								
			case PORT_P1: FIO1DIR |=mask;
						  if (value) FIO1SET = mask;
						  else       FIO1CLR = mask;
						  break;
		}
	}  
	else
	{
		switch (port)
		{
			case PORT_P0: IO0DIR |=mask;
						  if (value) IO0SET = mask;
						  else 		 IO0CLR = mask;
						  break;
						  
			case PORT_P1: IO1DIR |=mask;
						  if (value) IO1SET = mask;
						  else       IO1CLR = mask;
						  break;
		}
	}

}

void IO_output_low(int pin)
{
	IO_output(pin, 0);
}


void IO_output_high(int pin)
{
	IO_output(pin, 1);
}



int  IO_get_mode()
{
	return IO_mode;
}

#ifdef __cplusplus
}
#endif



