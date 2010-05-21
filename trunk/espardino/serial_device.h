#ifndef __serial_H
#define __serial_H

#include <buffer.h>
#include <micro214x_types.h>
#include <serial_device_defs.h>



typedef void (*thandler) (unsigned char);


 /* Initialize Serial Interface Driver */
void serial_init (void);               

void serial_set_uart0_handler(thandler uart0_handl);
void serial_set_uart1_handler(thandler uart1_handl);


/************************************************************* 
 * 				UART 0 access functions 					 *
 *************************************************************/

bool serial_putchar0 (unsigned char ch) ;  /* Write character to Serial Port    */
void serial_putchar0_blocking(unsigned char data);
void serial_flush0();

void serial_setopts0(unsigned int opts);
unsigned int serial_setbaud0(unsigned int baudrate);
void serial_clear_FIFO_0();


/************************************************************* 
 * 				UART 1 access functions 					 *
 *************************************************************/

bool serial_putchar1 (unsigned char ch) ;           /* Write character to Serial Port    */
void serial_putchar1_blocking(unsigned char data);
bool serial_sendbuffer1 (unsigned char *data, int len);
int serial_recvbuffer1 (unsigned char *data, int len);

void serial_flush_uart1_tx();
bool serial_getkey1_nonblock(unsigned char *data);


unsigned char serial_getkey1(void);
void serial_setopts1(unsigned int opts);
unsigned int serial_setbaud1(unsigned int baudrate);
unsigned int serial_num1();
	void serial_clear_FIFO_1();


#endif

