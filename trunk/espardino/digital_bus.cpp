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


#include <digital_bus.h>
#include <iolib.h>

/** DigitalBus constructor.
 *  you will have to attach to the pins later, \see attach 
 */
DigitalBus::DigitalBus()
{
	for (int i=0;i<DIGITALBUS_MAX_PIN;i++)
		p[i]=NO_PIN;
}
/**DigitalBus constructor, directly attached.
 * Every pin that we want to attach to this digital bus should be set in the
 * parameters, one by one, starting from bit0 to the last one.
 * @param p0 is the pin for bit0
 * @param p1 is the pin for bit1
 * @param p2,p3,p4,p5 is the pin for bit2
 */
DigitalBus::DigitalBus(int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7)
{
	this->attach(p0,p1,p2,p3,p4,p5,p6,p7);	
}

/**Attach to a series of pins.
 * \see DigitalBus constructor with pins
 */
int DigitalBus::attach(int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7)
{
	p[0]=p0; p[1]=p1; p[2]=p2; p[3]=p3;
	p[4]=p4; p[5]=p5; p[6]=p6; p[7]=p7;
	p[8]=NO_PIN;
	
	last_pin = DIGITALBUS_MAX_PIN-1;
	
	for (int i=0;i<DIGITALBUS_MAX_PIN;i++)
	{
		if (p[i]==NO_PIN) 
		{
			last_pin = i;
			break;
		}
	}
	return 1;

}

void DigitalBus::writeBitByBit(int data)
{
	
	/*
	 * Scan all bits in the "data" word to write all the
         * output signals
         */
	int bmask=1;
	for (int i=0;i<last_pin;i++)
	{
		IO_output(p[i],data&bmask);
		bmask=bmask<<1;
	}
	
}

void DigitalBus::writeFastByte(int data)
{

	/* this function is meant for 8bit fast output
	   on aligned 8bit ports, still unimplemented,
	   the optimizable condition must be detected on
	   attach(..) call */

	return;
}

int DigitalBus::readBitByBit()
{
	int data=0;
	int bmask=1;

	/*
   * Scan all bits in the input pins to read a word
   */
	for (int i=0;i<last_pin;i++)
	{
		if (IO_input(p[i])) data|=bmask;
		bmask=bmask<<1;
	}

	return data;
	
}
int DigitalBus::readFastByte()
{
	/* still unimplemented, same as writeFastByte */
	return 0;
}

/** Read data.
 *  this function reads the data from the bus of pins that we 
 *  described on the constructor or at attach time.
 *  @return the data from p0 to px in bits fron 0 to x of a word. 
 */
int DigitalBus::read()
{
	return readBitByBit();
}

/** Write data.
 * This function writes data to the bus of pins that we described 
 * on the constructor or at attach time.
 * @param data is the data to be written
 */
void DigitalBus::write(int data)
{
	last_val = data;
	writeBitByBit(data);
}
