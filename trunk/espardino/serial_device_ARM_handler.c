#include <LPC214x.h>                     /* LPC21xx definitions               */
#include "serial_device.h"

extern u8 		uart0_tx_buffer_data[PACKET_SIZE];
extern c_buffer uart0_tx_buffer;
extern u8 		uart1_tx_buffer_data[PACKET_SIZE];
extern c_buffer uart1_tx_buffer;

extern thandler uart0_rx_handler;
extern thandler uart1_rx_handler;


void serial_irq_handler(volatile unsigned char *UART,
						thandler rx_byte_handler,
						c_buffer *tx_buffer);

// define serial IRQ functions ////////////////////////////////////////////////
void serial_irq_handler_UART0 (void) __attribute__ ((interrupt("IRQ")));
void serial_irq_handler_UART1 (void) __attribute__ ((interrupt("IRQ")));

///////////////////////////////////////////////////////////////////////////////
//
// serial_irq_handler_UART0 (void)
// 
//

void serial_irq_handler_UART0 (void) {

	serial_irq_handler(UART0_BASE,
					   uart0_rx_handler,
					   &uart0_tx_buffer);	

	VICVectAddr = 0;       // Acknowledge Interrupt
}



///////////////////////////////////////////////////////////////////////////////
//
// serial_irq_handler_UART1 (void)
// 
//

void serial_irq_handler_UART1 (void) {

	serial_irq_handler(UART1_BASE,
					   uart1_rx_handler,
					   &uart1_tx_buffer);	

	VICVectAddr = 0;       // Acknowledge Interrupt

}

