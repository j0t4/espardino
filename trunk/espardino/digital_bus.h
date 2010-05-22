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



#ifndef __DIGITAL_BUS_H
#define __DIGITAL_BUS_H

#include <micro214x.h>

#define DIGITALBUS_MAX_PIN 16
class DigitalBus
{
   private:
	int p[DIGITALBUS_MAX_PIN];
	int last_pin;
	int last_val;
	int opt_level;
	int opt_register;
	void writeBitByBit(int data);
	void writeFastByte(int data);
	int readBitByBit();
	int readFastByte();

   public:
	DigitalBus();
	DigitalBus(int p0, int p1=NO_PIN,int p2=NO_PIN,int p3=NO_PIN,
                   int p4=NO_PIN,int p5=NO_PIN,int p6=NO_PIN,int p7=NO_PIN);

	int attach(int p0, int p1=NO_PIN,int p2=NO_PIN,int p3=NO_PIN,
                   int p4=NO_PIN,int p5=NO_PIN,int p6=NO_PIN,int p7=NO_PIN);

	int read();
	int hiz() { return read(); }
	void output() { write(last_val); }
	void write(int data);	
};

#endif
