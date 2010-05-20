

class Analog
{
   private:
	int precbits;
	int m_pin;
	float m_max;
	float m_min;
   public:
	int attach(int pin);
	int read();
	float readFloat();
	int setMaxFloat(float max);
	int setMinFloat(float min);
	int setPrecissionBits(int bits);
};	
	
/* class HasPrintf:
*
*  this class is useless by itself, but gives printf
*  functionality to any other Serial/LCD/.. class, only
*  putchar function needs to be implemented in child's class 
*
*/

class HasPrintf
{
public:
	int printf(char *fmt,...);
virtual int write(char c)=0;
};

#define USB_TX (-1)
#define USB_RX (-2)

#define GDB_TX (-3)
#define GDB_RX (-4)

#define U0_TX  (P0_2)
#define U0_RX  (P0_3)

#define U1_TX  (P0_33)
#define U1_RX  (P0_44)

class Serial : public HasPrintf
{

  private:
	int port;
	int bps;

  public:
	Serial();
	~Serial();
	
	int attach (int TX_pin, int RX_pin);

	/* send&receive functions: character */
	int send(char c);
	int recv();

	int write(char c); /* for printf compatibility */
	
	/* send&receive functions: buffer */
	int send(char *buffer,int len);
	int recv(char *buffer,int len);
	
	/* non blocking reception: won't wait if there is no data */
	int recvNonblocking();

	
	/* kbhit: checks if there is data waiting in read buffer */
	int kbhit();

	/* set bits per second */
	int setBps(int bps);
	int getBps() { return bps; }
    
};

#define SPI_NO_CS -1

#define SPI0 0
#define SPI1 1

class SPI
{
  private:

	int port;
	int bps;
  public:
	SPI();
	~SPI();
	int attach(int CLK_pin, int DAT_pin, int CS_pin);
	char xfer(char c);
	int xfer(char *buffer, int len);
	int setBps(int bps);
	int getBps() { return bps; }
};

class I2C
{	
  private:
 	int port;
	int state;
  public:
	I2C();
	int attach(int CLK_pin, int DAT_pin);
	
	/* start/stop I2C bus conditions */
	int start();
	int stop();

	/* read/write to the I2C bus */
	int read();	
	int write(char c);
	
	/* read/write using smbus protocol (addr|R/W  ... data ) */
	int smbus_write(char addr, char *data, int len);
	int smbus_read(char addr, char *data, int len);
	int smbus_write_8(char addr, char data);
	int smbus_write_16(char addr, int data);
	int smbus_read_8(char addr);
	int smbus_read_16(char addr);	
};	

class Led
{
  private: 
	int m_pin;
	int m_stat;
  public:
	Led();
	Led(int led) { attachLed(led); }
	int attach(int pin);
	int attachLed(int led); 
	void on();
	void off();
	int getStat();
};


	
/*
class Servo 
{
	public:		
		Servo();
		
		void move(double angle);
		void write(double angle) { this->move(angle); }
		int attach(int pin);
		void stop();
		
		int get_us() { return this->current_us; }
		int get_angle() { return this->current_angle; }
		
		int min_us;
		int max_us;
		
	private:
		
		int current_us;
		double current_angle;
		int pwm_pin;
	
};
*/

class FileRead
{
private:
	char *path;
	int status;
public:
	FileRead();
	~FileRead();
	int open(char *path);
	int readChar();	
	int readLine(char *buffer, int maxbuffer);
	int readBuffer(char *buffer, int len);
	int readU16();
	unsigned int readU32();
	int tell();
	int len();
	int seek(int pos);
	int seek(int pos,int whence);
	int close();
	
};

class FileWrite : public HasPrintf
{
private:
	char *path;
	int status;
public:
	FileWrite();
	~FileWrite();
	int open(char *path);
	int write(char c);	
	int writeLine(char *str);
	int writeBuffer(char *buffer, int len);
	int writeU16(int dat);
	int writeU32(unsigned int dat);
	int tell();
	/* should we implement this for writing? */

	int len();
	int seek(int pos);
	int seek(int pos,int whence);
	int close();
	
};

class Timer
{
private:
	int start_us;
public:
	Time();
	~Time();
virtual	void start();
virtual	void stop();
virtual	void reset();
virtual	int read();
virtual	int read_ms();
virtual	int read_us();

};

class FreeRTOSTimer : public Timer
{
	/* Reimplements all functions to use the
	   FreeRTOS subsystem */
};



class EINTHandler
{
   private:
	int m_pin;
	int m_vector;
	static int m_first_vector;
	ptVoidFunction m_function;

   public:
	IRQHandler();
	~IRQHandler();
	int attach(int pin, ptVoidFunction function, int isFIQ=0);	
	int detach();	
	int disable();
	int enable();
};




class DAC
{
public:
	DAC() {};
	void init();
	void write(int data);
};

#include <rtc.h>

class RealTimeClock
{
private:
	RTC data;
public:
	RealTimeClock();
	~RealTimeClock();

	int read();
	int write();

	int getYear();
	int getMonth();
	int getDay();
	int getDayOfWeek();
	int getHour();
	int getMinute();
	int getSecond();
	
	int setYear();	
	int setMonth();
	int setDay();
	int setHour();
	int setMinute();
	int setSecond();

};

class LCD : public HasPrintf
{
private:
	int interface_bits;
	DigitalBus bus;
	DigitalIO  rs,rw,en;
	int m_cols, m_lines, m_x,m_y;
public:
	LCD();
	~LCD();
	int attach(int p_RS, int p_RW, int p_EN, 
		   int p0, int p1,int p2,int p3,
                   int p4=NO_PIN,int p5=NO_PIN,int p6=NO_PIN,int p7=NO_PIN);
	int setup(int columns, int lines);
	int write(char dat);
	int putchar(char c); /* for printf compatibility */
	int write(char *str);
	int gotoXY(int x, int y);
	int printf(char *fmt,...);
	 	
};

/* we have to decide how to setup the network stuff classes .. */



