#ifndef __DIGITAL_BUS_H
#define __DIGITAL_BUS_H

#define NO_PIN -1
#define DIGITALBUS_MAX_PIN 16
class DigitalBus
{
   private:
	int p[DIGITALBUS_MAX_PIN];
	int last_pin;
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
	void write(int data);	
};

#endif
