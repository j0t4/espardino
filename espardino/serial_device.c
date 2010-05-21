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

#include <LPC214x.H>              
#include <serial_device.h>      
#include <sysclocks.h>


#define CR     0x0D

#define BR_115200 0x08
#define BR_460800 0x02

#define BR_UART0 BR_115200
#define BR_UART1 BR_115200


// define serial IRQ functions ////////////////////////////////////////////////
void serial_irq_handler_UART0 (void) __attribute__ ((interrupt("IRQ")));
void serial_irq_handler_UART1 (void) __attribute__ ((interrupt("IRQ")));
///////////////////////////////////////////////////////////////////////////////




unsigned char 		 uart0_tx_buffer_data[PACKET_SIZE];
c_buffer uart0_tx_buffer;


unsigned char 		 uart1_rx_buffer_data[PACKET_SIZE];
c_buffer uart1_rx_buffer;
unsigned char 		 uart1_tx_buffer_data[PACKET_SIZE];
c_buffer uart1_tx_buffer;



thandler uart0_rx_handler = NULL;
thandler uart1_rx_handler = NULL;



void serial_irq_handler(volatile unsigned char *UART,
						thandler rx_byte_handler,
						c_buffer *tx_buffer) {

	unsigned int source,status;
	unsigned char t_data,t;
			
	source = UART[IIR];

	if ((source&1)==0) {
		switch((source>>1)&0x07){

			case 0x03: // RLS (Receiver Line Status)
					 	status = UART[LSR];
						break;

			case 0x06: // CTI (Character Timeout Indicator)
			case 0x02: // RDA (Receiver Data Available)
						while (UART[LSR]&0x01) {
							if (rx_byte_handler) {
								rx_byte_handler(UART[RBR]);
							} else {
								t=UART[RBR];
							}
						}
						break;	
		
			case 0x01: // THRE (Transmit Hold Register ?Empty)
						
				
						while (UART[LSR] & 0x20) {
							if (buffer_get(tx_buffer,&t_data)==res_OK)
							{
								UART[THR] = t_data;		
							}	
							else
							{
								UART[IER] = 0x001;  // UART0 Interrupt Disable
								break;         // Register: bits 2=status 1=tx 0=rx
							}
						 }
						break;
		}
	}
	
}



void serial_rx_handler_uart1(unsigned char data) {

	buffer_put(&uart1_rx_buffer,data);
}

void serial_set_uart0_handler(thandler uart0_handl){

	uart0_rx_handler = uart0_handl;
}

void serial_set_uart1_handler(thandler uart1_handl){

	if (uart1_handl)
		uart1_rx_handler = uart1_handl;
	else
		uart1_rx_handler = &serial_rx_handler_uart1;
}

void serial_clear_FIFO(volatile unsigned char *UART) {

	UART[FCR] = 0x07; // enable FIFO, clear RX/TX // rx trigger level = 0

}

void serial_setup(volatile unsigned char *UART, unsigned char bitrate_const) {

  // uart 0 baudrate setup
  UART[LCR] = 0x83;                /* 8 bits, no Parity, 1 Stop bit     */
  UART[DLL] = bitrate_const;             /* Baud Rate @ 15MHz VPB Clock		*/
  UART[LCR] = 0x03;                /* DLAB = 0                          */

  serial_clear_FIFO(UART);

  UART[IER] = 0x01; // UART0 Interrupt Enable Register: bits 2=status 1=tx 0=rx

}

void serial_init (void)  {               /* Initialize Serial Interface*/


  serial_set_uart0_handler(NULL);
  serial_set_uart1_handler(NULL);

  // initialize the TX buffers
  buffer_init(&uart0_tx_buffer,uart0_tx_buffer_data,sizeof(uart0_tx_buffer_data));						
  buffer_init(&uart1_tx_buffer,uart1_tx_buffer_data,sizeof(uart1_tx_buffer_data));

  // initialize the RX buffers					
  buffer_init(&uart1_rx_buffer,uart1_rx_buffer_data,sizeof(uart1_rx_buffer_data));


  // setup the UART0 IRQ handler and enable 
  VICVectAddr7   = (unsigned int)serial_irq_handler_UART0;
  VICVectCntl7   =  0x0026; // enable int source 6 (UART0)
  
  // setup the UART0 IRQ handler and enable 
  VICVectAddr8   = (unsigned int)serial_irq_handler_UART1;
  VICVectCntl8   =  0x0027; // enable int source 7 (UART1)

  serial_setup(UART0_BASE,BR_UART0);
  serial_setup(UART1_BASE,BR_UART1);


  VICIntEnable   = 0x000000C0;
}


bool serial_putchar(volatile unsigned char *UART,c_buffer *tx_buffer, unsigned char ch) {
	
	int result;

	// if transmission resgister isn't free....
  	if (buffer_num(tx_buffer)>0||(!(UART[LSR] & 0x20))) {	

  		do {
			// we wait to put it in the buffer..
			result = buffer_put(tx_buffer,ch);
			UART[IER] = 0x003;
		}	while (result != res_OK);

  	} else {	  			    	
  		UART[THR] = ch;		// if register is free...
 		UART[IER] = 0x003;	// we just transmit it
  	}

  	return true;
}

unsigned int serial_setbaud(volatile unsigned char *UART, unsigned int baudrate) {

	unsigned int BR_constant;
	unsigned int real_baudrate;
	unsigned char lcr_data;

	BR_constant = VPB_get_speed() / (16*baudrate);
	real_baudrate = VPB_get_speed() / (16*BR_constant);
	
	lcr_data = UART[LCR];
	UART[LCR] = lcr_data|LCR_DLAB;
	UART[DLM] = (BR_constant>>8) & 0xff;/* baud rate constant 				*/
  	UART[DLL] = (BR_constant   ) & 0xff;/*  ""   ""     ""					*/
  	UART[LCR] = lcr_data&(~LCR_DLAB); 	/* DLAB = 0       					*/

	return real_baudrate;
}

void serial_setopts(volatile unsigned char *UART, unsigned int opts) {
   UART[LCR] = opts&(~LCR_DLAB );		
}




/************************************************************* 
 * 				UART 0 access functions 					 *
 *************************************************************/


bool serial_putchar0 (unsigned char ch)  {  /* Write character to Serial Port    */
  
  	return 	serial_putchar(UART0_BASE,&uart0_tx_buffer,ch);
}


void serial_putchar0_blocking(unsigned char data) {

		while (!(U0LSR & 0x20)); // wait until UART0 has space in FIFO
		U0THR = data;	 // put the data in FIFO
}

void serial_clear_FIFO_0() {
 serial_clear_FIFO(UART0_BASE);
}

void serial_flush0() {

	unsigned char t_data;

	while (buffer_get(&uart0_tx_buffer,&t_data)==res_OK) 
		serial_putchar0_blocking(t_data);

}

void serial_setopts0(unsigned int opts) {
	serial_setopts(UART0_BASE,opts);

}

unsigned int serial_setbaud0(unsigned int baudrate) {

	return serial_setbaud(UART0_BASE,baudrate);
}

/************************************************************* 
 * 				UART 1 access functions 					 *
 *************************************************************/

bool serial_putchar1 (unsigned char ch)  {           /* Write character to Serial Port    */
	
	return serial_putchar(UART1_BASE,&uart1_tx_buffer,ch);
}

bool serial_sendbuffer1 (unsigned char *data, int len) {

 	while (len--) serial_putchar1(*data++);
	return true;
}

void serial_putchar1_blocking(unsigned char data) {

		while (!(U1LSR & 0x20)); // wait until UART0 has space in FIFO

		U1THR = data;	 // put the data in FIFO
}

void serial_flush_uart1_tx() {

	unsigned char t_data;

	while (buffer_get(&uart1_tx_buffer,&t_data)==res_OK)
		serial_putchar1_blocking(t_data);
		
}

bool serial_getkey1_nonblock(unsigned char *data) {

	return (buffer_get(&uart1_rx_buffer,data)==res_OK);
}





unsigned char serial_getkey1(void) 
{		
	unsigned char r_data;

	while (buffer_get(&uart1_rx_buffer,&r_data)!=res_OK);  //bloqueante

	return r_data;	

}

int serial_recvbuffer1 (unsigned char *data, int len) {
	
	int rlen = len;
 	while (len--) *data++=serial_getkey1();

	return rlen;
}

void serial_setopts1(unsigned int opts) {
	serial_setopts(UART1_BASE,opts);

}
unsigned int serial_setbaud1(unsigned int baudrate) {
	
	return serial_setbaud(UART1_BASE,baudrate);
}

unsigned int serial_num1() {

	  return buffer_num(&uart1_rx_buffer);

}

	void serial_clear_FIFO_1() {
 serial_clear_FIFO(UART1_BASE);
}






