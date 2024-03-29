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

#include <LPC214x.h>              
#include <serial_device.h>      
#include <sysclocks.h>
#include <arm_irqs.h>


#define CR     0x0D

#define BR_UART0 115200
#define BR_UART1 115200


// define serial IRQ functions ////////////////////////////////////////////////
void serial_irq_handler_UART0 (void) __attribute__ ((interrupt("IRQ")));
void serial_irq_handler_UART1 (void) __attribute__ ((interrupt("IRQ")));
///////////////////////////////////////////////////////////////////////////////




unsigned char 		 uart0_tx_buffer_data[PACKET_SIZE];
c_buffer uart0_tx_buffer;

unsigned char 		 uart0_rx_buffer_data[PACKET_SIZE];
c_buffer uart0_rx_buffer;

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


void serial_rx_handler_uart0(unsigned char data) {

	buffer_put(&uart0_rx_buffer,data);
}

void serial_rx_handler_uart1(unsigned char data) {

	buffer_put(&uart1_rx_buffer,data);
}

void serial_set_uart0_handler(thandler uart0_handl){

	if (uart0_handl)
		uart0_rx_handler = uart0_handl;
	else
		uart0_rx_handler = &serial_rx_handler_uart1;
}

void serial_set_uart1_handler(thandler uart1_handl){

	if (uart1_handl)
		uart1_rx_handler = uart1_handl;
	else
		uart1_rx_handler = &serial_rx_handler_uart1;
}

void serial_clear_FIFO_(volatile unsigned char *UART) 
{

	UART[FCR] = 0x07; // enable FIFO, clear RX/TX // rx trigger level = 0

}

void serial_setup(volatile unsigned char *UART, unsigned char bitrate_const) {

  // uart 0 baudrate setup
  UART[LCR] = 0x83;                /* 8 bits, no Parity, 1 Stop bit     */
  UART[DLL] = bitrate_const;             /* Baud Rate @ 15MHz VPB Clock		*/
  UART[LCR] = 0x03;                /* DLAB = 0                          */

  serial_clear_FIFO_(UART);

  UART[IER] = 0x01; // UART0 Interrupt Enable Register: bits 2=status 1=tx 0=rx

}

unsigned int serial_setbaud_(volatile unsigned char *UART, unsigned int baudrate);


bool serial_putchar_(volatile unsigned char *UART,c_buffer *tx_buffer, unsigned char ch) {
	
	int result;

	// if transmission register isn't free....
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

unsigned int serial_setbaud_(volatile unsigned char *UART, unsigned int baudrate) {

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

void serial_setopts_(volatile unsigned char *UART, unsigned int opts) {
   UART[LCR] = opts&(~LCR_DLAB );		
}


void serial_init (int channel,int init_pins)  {               /* Initialize Serial Interface*/


	if (channel==0)
	{
		if (init_pins & SERIAL_INIT_RX)
		{
				PINSEL0 = (PINSEL0 & (~(3<<2)))|(1<<2); /* P0.1 as RXD0 */
		}
		
		if (init_pins & SERIAL_INIT_TX)
		{
			PINSEL0 = (PINSEL0 & (~3))|1;		        /* P0.0 as TXD0 */
		}
		
		
	  serial_set_uart0_handler(NULL);
	  buffer_init(&uart0_tx_buffer,uart0_tx_buffer_data,sizeof(uart0_tx_buffer_data));						
	  buffer_init(&uart0_rx_buffer,uart1_rx_buffer_data,sizeof(uart0_rx_buffer_data));
	  serial_setup(UART0_BASE,0);
	  serial_setbaud_(UART0_BASE,BR_115200);
	  VIC_setup_irq(6, serial_irq_handler_UART0);
	}
	else if (channel==1)
	{	  
  
  	if (init_pins & SERIAL_INIT_RX)
		{
  		PINSEL0 = (PINSEL0 & (~(3<<18)))|(1<<18);		/* P0.9 as RXD1 */
  	}
  	
  	if (init_pins & SERIAL_INIT_TX)
		{
  		PINSEL0 = (PINSEL0 & (~(3<<16)))|(1<<16);   /* P0.8 as TXD1 */
  	}
  	
		
		
  	serial_set_uart1_handler(NULL);
		buffer_init(&uart1_tx_buffer,uart1_tx_buffer_data,sizeof(uart1_tx_buffer_data));
  	buffer_init(&uart1_rx_buffer,uart1_rx_buffer_data,sizeof(uart1_rx_buffer_data));
  	serial_setup(UART1_BASE,0);
  	serial_setbaud_(UART1_BASE,BR_115200);
  	VIC_setup_irq(7, serial_irq_handler_UART1);
	}
  

}

            
void serial_set_handler(int channel, thandler uart_handl)
{
	if (channel==0) serial_set_uart0_handler(uart_handl);
	if (channel==1) serial_set_uart1_handler(uart_handl);
}

bool serial_putchar (int channel, unsigned char ch) 
{
	if (channel==0) return 	serial_putchar_(UART0_BASE,&uart0_tx_buffer,ch);
	if (channel==1) return 	serial_putchar_(UART1_BASE,&uart1_tx_buffer,ch);
	return 0;
}


unsigned int serial_setbaud (int channel,unsigned int baudrate)
{
	if (channel==0) return serial_setbaud_(UART0_BASE,baudrate);
	if (channel==1) return serial_setbaud_(UART1_BASE,baudrate);
	return 0;
}


void serial_clear_FIFO(int channel)
{
	if (channel==0) return serial_clear_FIFO_(UART0_BASE);
	if (channel==1) return serial_clear_FIFO_(UART1_BASE);
}


bool serial_sendbuffer (int channel, unsigned char *data, int len)
{
	while (len--) serial_putchar(channel, *data++);
	return true;
}

int serial_recvbuffer (int channel, unsigned char *data, int len)
{
	while (len--) 
	{
		*data++= serial_getchar(channel);
	}
	return true;
}

bool serial_getchar_nonblock(int channel, unsigned char *data)
{
	if (channel==0) return (buffer_get(&uart0_rx_buffer,data)==res_OK);
	if (channel==1) return (buffer_get(&uart1_rx_buffer,data)==res_OK);
	return 0;
}

unsigned char serial_getchar(int channel)
{
	unsigned char r_data=0xff;

	if (channel==0)  { while (buffer_get(&uart0_rx_buffer,&r_data)!=res_OK);  }
	if (channel==1)  { while (buffer_get(&uart1_rx_buffer,&r_data)!=res_OK);  }
		
	return r_data;	
	
}

unsigned int serial_num(int channel)
{
	 if (channel==0) return buffer_num(&uart1_rx_buffer);
	 if (channel==1) return buffer_num(&uart1_rx_buffer);
		return -1;
}
	

