#include <serial.h>
#include <vcom.h>
#include <serial_device.h>

/*
#define USB_TX (-1)
#define USB_RX (-2)

#define GDB_TX (-3)
#define GDB_RX (-4)


#define U0_TX  (P0_2)
#define U0_RX  (P0_3)

#define U1_TX  (P0_33)
#define U1_RX  (P0_44)


*/ 

bool Serial::usb_initialized = false;

Serial::Serial()
{
	bps = 115200;		
	rx_port = NO_PIN;
	tx_port = NO_PIN;
	gdb_use_buffering=false;
}

Serial::Serial(int TX_pin, int RX_pin)
{
	bps = 115200;		
	rx_port = NO_PIN;
	tx_port = NO_PIN;
	gdb_use_buffering=false;
	attach(TX_pin,RX_pin);
}


Serial::~Serial()
{
}

bool Serial::detect_gdbmonitor(void)
{
	unsigned int *p=(unsigned int*)0x203c;
	if (p[0]==0xdebdebde)
	{
		gdb_putc =  (putCharFunc)(p[2]);
		gdb_puts =  (putStrFunc) (p[3]);
		gdb_buffer_n=0;
		return true;
	}
	else
	{
		gdb_putc=NULL;
		gdb_puts=NULL;
		return false;
	}
}
int Serial::attach (int TX_pin, int RX_pin)
{
	/* store for later use */
	
	if ((TX_pin==USB_TX && RX_pin==NO_PIN)||
		  (TX_pin==USB_TX && RX_pin==USB_RX)||
		  (TX_pin==NO_PIN && RX_pin==USB_RX))
 	{

		if (detect_gdbmonitor()) /* we don't want to init USB if the gdb monitor is there */
		{
			RX_pin = GDB_RX;
			TX_pin = GDB_TX;
		} else if (usb_initialized == false)
 		{
 			VCOM_init();
 			usb_initialized=true;
 		}

		rx_port = RX_pin;
		tx_port = TX_pin;		
			
		return 1;
	}
	
	if ((TX_pin==U0_TX && RX_pin==NO_PIN)||
		  (TX_pin==U0_TX && RX_pin==U0_RX)||
		  (TX_pin==NO_PIN && RX_pin==U0_RX))
 	{
 		int pinsetup=0;
 		
 		if (TX_pin!=NO_PIN) pinsetup|=SERIAL_INIT_TX;
 		if (RX_pin!=NO_PIN) pinsetup|=SERIAL_INIT_TX;
 			
 		serial_init(0,pinsetup);
 		
 		rx_port = RX_pin;
		tx_port = TX_pin;		
 			
 		return 1;
	}
	
	if ((TX_pin==U1_TX && RX_pin==NO_PIN)||
		  (TX_pin==U1_TX && RX_pin==U1_RX)||
		  (TX_pin==NO_PIN && RX_pin==U1_RX))
 	{
 		int pinsetup=0;
 		
 		if (TX_pin!=NO_PIN) pinsetup|=SERIAL_INIT_TX;
 		if (RX_pin!=NO_PIN) pinsetup|=SERIAL_INIT_TX;
 			
 		serial_init(1,pinsetup);
 		
 		rx_port = RX_pin;
		tx_port = TX_pin;		
 			
 		return 1;
	}
	
	if (TX_pin==GDB_TX)
	{
		return 0;
	}
	
	return -1;
	
}

void Serial::useBuffer(bool use_buf)
{
	gdb_use_buffering = use_buf;
}

int Serial::GDB_putchar(char c)
{
	if (gdb_use_buffering)
	{
		gdb_buffer[gdb_buffer_n++]=c;
	
		if ((c=='\0')||(c=='\n')||(gdb_buffer_n>=(GDB_BUFFER_SIZE-1)))
		{
			gdb_buffer[gdb_buffer_n]='\0';
			gdb_puts(gdb_buffer);
			gdb_buffer_n=0;
		}
	}
	else
	{
		gdb_putc(c);
	}
return 1;
}

int Serial::send(char c)
{
	switch (tx_port)
	{
		case U0_TX: return serial_putchar(0,c);
		case U1_TX: return serial_putchar(1,c);
		case USB_TX: return VCOM_putchar(c);
		case GDB_TX: return GDB_putchar(c);
		default: return -1;
	}
			
}	

int Serial::recv()
{
	switch(rx_port)
	{
		case U0_RX: return serial_getchar(0);
		case U1_RX: return serial_getchar(1);
		case USB_RX: return VCOM_getchar();
		default: return -1;
	}
}

int Serial::puts(char *str)
{
	int len=0;
	while(*str)
	{
		 send(*str++);
		 len++;
	}
	
	return len;
}

int Serial::send(char *buffer,int len)
{
	int old_len=len;
	while (len--) send(*buffer++);
	return old_len;
}
int Serial::recv(char *buffer,int len)
{
	int old_len=len;
	while (len--) *buffer++ = (char) recv();
	return old_len;
}
	
	
/* non blocking reception: won't wait if there is no data */
int Serial::recvNonblocking()
{
	unsigned char data;
	switch(rx_port)
	{
		case U0_RX: if (serial_getchar_nonblock(0,&data)) return (int)data;
							  else return -1;
		case U1_RX: if (serial_getchar_nonblock(1,&data)) return (int)data;
							  else return -1;
		case USB_RX: return VCOM_getchar_nonblock();
		default: return -1;
	}
}

	
	/* kbhit: checks if there is data waiting in read buffer */
int Serial::kbhit()
{
		switch(rx_port)
	{
		case U0_RX: return serial_num(0);
		case U1_RX: return serial_num(1);
		case USB_RX: return VCOM_kbhit();
		default: return 0;
	}

}

	/* set bits per second */
int Serial::setBps(int bps)
{
	this->bps=bps;
	if ((tx_port==U0_TX)||(rx_port==U0_RX))
	{
		serial_setbaud(0,bps);
	}
	
	if ((tx_port==U1_TX)||(rx_port==U1_RX))
	{
		serial_setbaud(1,bps);
	}
	
	return bps;
	
}


    

