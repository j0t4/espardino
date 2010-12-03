#ifndef __SERIAL_H
#define __SERIAL_H

#include <micro214x.h>
#include <hasprintf.hpp>

#define USB_TX (-1)
#define USB_RX (-2)

#define GDB_TX (-3)
#define GDB_RX (-4)


#define U0_TX  (P0_0)
#define U0_RX  (P0_1)

#define U1_TX  (P0_8)
#define U1_RX  (P0_9)

typedef void(*putCharFunc)(char);
typedef void(*putStrFunc)(char*);

class Serial : public HasPrintf
{

  private:
	int rx_port,tx_port;
	int bps;

	static bool usb_initialized;

	putCharFunc gdb_putc;
	putStrFunc  gdb_puts;

	bool detect_gdbmonitor(void);

#define GDB_BUFFER_SIZE 16
	int gdb_use_buffering;
	char gdb_buffer[GDB_BUFFER_SIZE];
	int gdb_buffer_n;
	int GDB_putchar(char c);

  public:
	Serial();
	Serial(int TX_pin=NO_PIN, int RX_pin=NO_PIN);
	~Serial();

	int attach (int TX_pin=NO_PIN, int RX_pin=NO_PIN);

	/* send&receive functions: character */
	int send(char c);
	int recv();
	int write(char c) { return send(c); } /* for printf compatibility */

	int putc(char c) { return write(c); }
	int getc() { return recv(); }

	int puts(char* str);
	int puts(const char* str) { return puts((char*)str); }
	char* gets(char *buf, int maxLen=2048);

	/* use buffering for GDB/others*/
	void useBuffer(bool use_buffer);

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

#endif
