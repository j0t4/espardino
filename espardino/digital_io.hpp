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


#ifndef __DIGITAL_IO_H
#define __DIGITAL_IO_H
#include <iolib.h>

class DigitalIO
{
   private:
	int m_pin; int last_val;
   public:
  	DigitalIO() 	     { m_pin=-1; }
	DigitalIO(int pin)   { m_pin=pin;}
	int attach(int pin)  { m_pin=pin; return 1; }
	int read()	     { return IO_input(m_pin); }
	void write(int data) { last_val=data; IO_output(m_pin,data); }
	void on() 	     { IO_output_high(m_pin); }
	void off()           { IO_output_low(m_pin); }
	void hiz()					 { IO_input(m_pin);      }
	void output()				 { IO_output(m_pin,last_val); }
	DigitalIO& operator= (	int 	val) { write(val); return *this; }
	operator int() { return IO_input(m_pin); }
};

#endif
