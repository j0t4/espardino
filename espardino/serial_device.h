#ifndef __serial_H
#define __serial_H

#include <buffer.h>
#include <micro214x_types.h>
#include <serial_device_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*thandler) (unsigned char);

 /* Initialize Serial Interface Driver */
 
 #define SERIAL_INIT_RX 1
 #define SERIAL_INIT_TX 2
 
void serial_init (int channel,int init_pins);               
void serial_set_handler(int channel, thandler uart_handl);
bool serial_putchar (int channel, unsigned char ch) ;  /* Write character to Serial Port    */
unsigned int serial_setbaud(int channel,unsigned int baudrate);
bool serial_sendbuffer (int channel, unsigned char *data, int len);
int serial_recvbuffer (int channel, unsigned char *data, int len);
bool serial_getchar_nonblock(int channel, unsigned char *data);
unsigned char serial_getchar(int channel);
unsigned int serial_num(int channel);
	
#ifdef __cplusplus
}
#endif

#endif

