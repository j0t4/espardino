#ifndef __serial_DEFS_H
#define __serial_DEFS_H

#define PACKET_SIZE 64

// UART addresses
#define UART0_BASE ((volatile unsigned char *) 0xE000C000)
#define UART1_BASE ((volatile unsigned char *) 0xE0010000)

// UART register offsets	
#define RBR       0x00
#define THR       0x00
#define IER       0x04
#define IIR       0x08
#define FCR       0x08
#define LCR       0x0C
#define LSR       0x14
#define SCR       0x1C
#define DLL       0x00
#define DLM       0x04
#define TER       0x30


#define BR_57600 0x10
#define BR_115200 0x08
#define BR_460800 0x02


#define LCR_5bit   0
#define LCR_6bit   1
#define LCR_7bit   2
#define LCR_8bit   3

#define LCR_1stop  (0<<2)
#define LCR_2stop  (1<<2)

#define LCR_parityenable (0<<3)
#define LCR_parityO	 (0<<4)
#define LCR_parityE  (1<<4)
#define LCR_parity1  (2<<4)
#define LCR_parity0  (3<<4)

#define LCR_break    (1<<6)
#define LCR_DLAB	 (1<<7)



#endif
