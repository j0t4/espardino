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
