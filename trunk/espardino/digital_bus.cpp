
#include  <digital_bus.h>
#include <iolib.h>

DigitalBus::DigitalBus()
{
	for (int i=0;i<DIGITALBUS_MAX_PIN;i++)
		p[i]=NO_PIN;
}

DigitalBus::DigitalBus(int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7)
{
	this->attach(p0,p1,p2,p3,p4,p5,p6,p7);	
}

int DigitalBus::attach(int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7)
{
	p[0]=p0; p[1]=p1; p[2]=p2; p[3]=p3;
	p[4]=p4; p[5]=p5; p[6]=p6; p[7]=p7;
	
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
	for (int i=0;i<=last_pin;i++)
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
	for (int i=0;i<=last_pin;i++)
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

int DigitalBus::read()
{
	return readBitByBit();
}

void DigitalBus::write(int data)
{
	writeBitByBit(data);
}
