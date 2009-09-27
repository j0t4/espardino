/*-----------------------------------------------------------------------*/
/* MMC/SDSC/SDHC (in SPI mode) control module for LPC2xxx SSP            */
/* (C) Martin Thomas, 2009 - based on the AVR module (C)ChaN, 2007       */
/* (C) NBEE Embedded Systems SL, 2009 - some parts, SPI0 instead of SSP  */
/*-----------------------------------------------------------------------*/

/* Copyright (c) 2009, Martin Thomas, ChaN
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/*-----------------------------------------------------------------------*/
/*
 * Martin Thomas 5/2009
 * - inspired by the EFSL interface-driver by Mike Anton and me
 * - using ChaN's driver-skeleton from his AVR mmc.c
 * - This code is for the SSP-Interface ("SPI1") and fast-IO (FIO).
 *   It is not prepared for "old style" SPI or "legacy/slow" IO
 * - Olimex LPC-P2148: P0.17 SD-Card clock, P0.18 from SD-Card,
     P0.19 to SD-Card, P0.20 SD-Card select - NO ext. pull-up mounted
     P1.24 write protect switch (ext. pull-up, trace must be turned off),
     P1.25 card-detect switch (ext. pull-up, trace must be turned off)
*/
/*-----------------------------------------------------------------------*/

#define USE_SOCKSWITCHES  1
#define USE_POWERCONTROL  0 /* not available on Olimex LPC-P2148 */
#define USE_FIFO          0

#include "diskio.h"
#include <LPC214x.h>
#include <iolib.h>

#define TIMER_EQV 50

//#define DEBUG_ESPARDINO_SDHC	1

/*--------------------------------------------------------------------------

   Module private variables and definitions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

#define SOCKWP		0x20			/* Write protect bit-mask (Bit5 set = */
#define SOCKINS		0x10			/* Card detect bit-mask   */

static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static 
unsigned int Timer1, Timer2;	/* 100Hz decrement timer */

static
BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */


/*--------------------------------------------------------------------------

   Module private portability Functions & Definitions ( Martin Thomas )

---------------------------------------------------------------------------*/

#if USE_SOCKSWITCHES
// PINSEL not available for these pins, make sure
// that trace is disable to use them for GPIO
#ifndef SD_SOCKET_WP_PIN
// Pin connected to the Write-Protect WP switch of the card socket
#define SD_SOCKET_WP_PIN       24
#define SD_SOCKET_WP_DIR       FIO1DIR
#define SD_SOCKET_WP_IN        FIO1PIN
#endif
#ifndef SD_SOCKET_INS_PIN
/// Pin connected to the Insert/Card Detect switch of the card socket
#define SD_SOCKET_INS_PIN       25
#define SD_SOCKET_INS_DIR       FIO1DIR
#define SD_SOCKET_INS_IN        FIO1PIN
#endif

static inline BYTE get_SOCKWP(void)
{
	//return ( SD_SOCKET_WP_IN & (1UL << SD_SOCKET_WP_PIN) ) ? SOCKWP /*protected*/ : 0 /* unprotected */;
	return 0; /* unprotected - unsuported in Espardino */
}
static inline BYTE get_SOCKINS(void)
{
	//return ( SD_SOCKET_INS_IN & (1UL <<SD_SOCKET_INS_PIN) ) ?  SOCKINS /*no card*/ : 0 /* card inserted */;
	return IO_input(P0_3);
}

static void init_SOCKWP_SOCKINS( void )
{
	/*SD_SOCKET_INS_DIR &= ~(1UL << SD_SOCKET_INS_PIN);
	SD_SOCKET_WP_DIR &= ~(1UL << SD_SOCKET_WP_PIN);*/
	
	if (IO_get_mode()==FAST_IO) 
	{
		FIO0DIR &= ~(1<<7) ;
	}
	else
	{
		IODIR0 &= ~(1<<7);
	}

}

#else /* USE_SOCKSWITCHES == 0 */

static inline BYTE get_SOCKWP(void)
{
	return 0; /* fake unprotected */
}
static inline BYTE get_SOCKINS(void)
{
	return 0; /* fake inserted */
}
static void init_SOCKWP_SOCKINS( void )
{
	return;
}

#endif /* USE_SOCKSWITCHES */


#if USE_POWERCONTROL
#error USE_POWERCONTROL not implemented
static inline void set_SOCKPOWER( BOOL on )
{
	pseudo-code
	if ( on ) {
		TODO enable power (p-channel fet)
	}
	else {
		disable power
	}
}
// returns TRUE if power is enabled
static inline BOOL get_SOCKPOWER( void )
{
	TODO return xxx;
}
static void init_SOCKPOWER( void )
{
	TODO output to FET gate
}
#else
static inline void set_SOCKPOWER( BOOL on )
{
	on = on;
	return;
}
// returns TRUE if power is enabled
static inline BOOL get_SOCKPOWER( void )
{
	return TRUE;
}
static void init_SOCKPOWER( void )
{
	return;
}

#endif /* USE_POWERCONTROL */


/*-----------------------------------------------------------------------*/
/* SPI-Interface (Platform dependent, mthomas)                           */
/*-----------------------------------------------------------------------*/

#define SSPCR0_DSS      0
#define SSPCR0_CPOL     6
#define SSPCR0_CPHA     7
#define SSPCR0_SCR      8
#define SSPCR1_SSE      1
#define SSPSR_TNF       1
#define SSPSR_RNE       2
#define SSPSR_BSY       4

#define PCONP_PCSPI1    10

#define SD_CS_BIT       20
#define SD_CS_DIR       FIO0DIR
#define SD_CS_SET       FIO0SET
#define SD_CS_CLR       FIO0CLR
#define SD_CS_SEL       PINSEL1
#define SD_CS_SEL_BIT   8

/* SPI0 - mapelayo */
// SP0SPCR  Bit-Definitions
#define CPHA    3
#define CPOL    4
#define MSTR    5
// SP0SPSR  Bit-Definitions
#define SPIF	7

// bit-frequency = PCLK / (CPSDVSR * [SCR+1]), here SCR=0, PCLK=60MHz, must be even
#define SPI_SPEED_20MHz		4	/* => 15MHz */
#define SPI_SPEED_25MHz		4	/* => 15MHz */
#define SPI_SPEED_400kHz  150	/* => 400kHz */


// select card ( MMC CS = L )
static inline void SELECT( void )
{
	/*SD_CS_CLR = ( 1UL << SD_CS_BIT );*/
	
	if (IO_get_mode()==FAST_IO) 
	{
		FIO0CLR= (1<<7);
	}
	else
	{
		IOCLR0 = (1<<7);
	}
}

// unselect card ( MMC CS = H )
static inline void DESELECT( void )
{
	/* SD_CS_SET = ( 1UL << SD_CS_BIT );*/
	if (IO_get_mode()==FAST_IO) 
	{
		FIO0DIR|= (1<<7);
		FIO0SET= (1<<7);
	}
	else
	{
		IO0DIR|= (1<<7);
		IOSET0 = (1<<7);
	}
}


static void spiSetSpeed(BYTE speed)
{
	speed &= 0xFE;
	if ( speed < 8  ) {
		speed = 8 ;
	}
	
	S0SPCCR = speed;  /* SPI0*/
	/*SSPCPSR = speed;*/
}

/* general SPI send/receive */
static inline BYTE spi_write_read( BYTE outgoing )
{
	BYTE incoming;

	/*
	SSPDR = outgoing;
	while( !( SSPSR & ( 1 << SSPSR_RNE ) ) ) { ; }
	incoming = SSPDR;
	*/
	
	S0SPDR = outgoing;
	while( !(S0SPSR & (1<<SPIF)) ) ;
	incoming = S0SPDR;

	return incoming;
}


#if USE_FIFO
/* on LPC2k the FIFOs have 8 elements which each can hold up to 16bits */
#define FIFO_ELEM 8

static inline void spi_rcvr_block (
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
   // SPI0 implentation (no FIFO)
   int i;
   for (i=0;i<btr;i++)
   {
		*buff++ = spi_write_read(0xff);
   }
/*
	UINT hwtr, startcnt, i, rec;

	hwtr = btr/2;
	if ( btr < FIFO_ELEM ) {
		startcnt = hwtr;
	} else {
		startcnt = FIFO_ELEM;
	}

	SSPCR0 |= 0x0000000f;  // DSS to 16 bit

	for ( i = startcnt; i; i-- ) {
		SSPDR = 0xffff;  // fill TX FIFO
	}

	do {
		while ( !(SSPSR & ( 1 << SSPSR_RNE ) ) ) {
			// wait for data in RX FIFO (RNE set)
		}
		rec = SSPDR;
		if ( i < ( hwtr - startcnt ) ) {
			SSPDR = 0xffff;
		}
		*buff++ = (BYTE)(rec>>8);
		*buff++ = (BYTE)(rec);
		i++;
	} while ( i < hwtr );

	SSPCR0 &= ~0x00000008;  // DSS to 8 bit */
} 

static inline void spi_xmit_block (
	const BYTE *buff	/* 512 byte data block to be transmitted */
)
{
	UINT cnt;
	
	/* SPI0 no FIFO implementation */
	for (cnt=0;cnt<512;cnt++) 
		spi_write_read(*buff++);
/*
	WORD data;

	SSPCR0 |= 0x0000000f;  // DSS to 16 bit

	for ( cnt = 0; cnt < ( 512 / 2 ); cnt++ ) {
		while ( !( SSPSR & ( 1 << SSPSR_TNF ) ) ) {
			// wait for TX FIFO not full (TNF)
		}
		data  = (*buff++) << 8;
		data |= *buff++;
		SSPDR = data;
	}

	while ( SSPSR & ( 1 << SSPSR_BSY ) ) {
		// wait for BSY gone
	}
	while ( SSPSR & ( 1 << SSPSR_RNE ) ) {
		data = SSPDR; // drain receive FIFO
	}

	SSPCR0 &= ~0x00000008;  // DSS to 8 bit
*/
}
#endif /* USE_FIFO */


static void init_spi( void )
{
/*	UINT dummy; */

	// configure CS as GPIO output and deselect card
	
	/*
	SD_CS_SEL = ( SD_CS_SEL & ~( 3UL << SD_CS_SEL_BIT ) );
	SD_CS_DIR |= (1UL << SD_CS_BIT);
    */
	
	DESELECT();

/* SPI1 
	// enable SSP power
	PCONP |= (1UL << PCONP_PCSPI1);

	// reset pin-functions
	PINSEL1 &= ~(
		(3UL << 2) | // SCK
		(3UL << 4) | // MISO
		(3UL << 6) | // MOSI
		(3UL << 8)   // SSEL
	);
	// set pin-functions to SSP
	PINSEL1 |= (
		(2UL << 2) | // SCK
		(2UL << 4) | // MISO
		(2UL << 6)   // MOSI
		// slave-select handled though GPIO
	);

	// configure and enable as SPI-Master
	SSPCR0 = ((8-1) << SSPCR0_DSS) | (0 << SSPCR0_CPOL) |
		(0 << SSPCR0_CPHA ) | (0 << SSPCR0_SCR);
	SSPCR1 = (1 << SSPCR1_SSE);
*/

	// reset pin-functions
	PINSEL0 &= ~(
		(3UL << 8) | // SCK
		(3UL << 10) | // MISO
		(3UL << 12) | // MOSI
		(3UL << 14)   // SSEL
				);
	// set pin-functions to SSP
	PINSEL0 |= 
		(1UL << 8) | // SCK
		(1UL << 10) | // MISO
		(1UL << 12) ;  // MOSI
		// slave-select handled though GPIO
			
	S0SPCR = (1<<MSTR)|(0<<CPOL);   /* enable as master, set polarity */

	// slow during initialization
	spiSetSpeed( SPI_SPEED_400kHz );

/* SPI1
	// wait for SSP idle)
	while( SSPSR & ( 1 << SSPSR_BSY ) ) {
		// wait for busy gone
	}

	while( SSPSR & ( 1 << SSPSR_RNE ) ) {
		dummy = SSPDR; // drain SPI RX FIFO
	}
*/
}


static void close_spi( void )
{

	S0SPCR = 0;
	
 	PINSEL0 &= ~(
		(3UL << 8) | // SCK
		(3UL << 10) | // MISO
		(3UL << 12) | // MOSI
		(3UL << 14)   // SSEL
				);
				
/* SPI1
	// disable SPI
	SSPCR1 = 0;

	// Pins to GPIO
	PINSEL1 &= ~(
		(3UL << 2) | // SCK
		(3UL << 4) | // MISO
		(3UL << 6) | // MOSI
		(3UL << 8)   // SEL
	);

	// disable SSP power
	PCONP &= ~(1UL << PCONP_PCSPI1);
*/

}



/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

static inline void xmit_spi( BYTE dat )
{
	spi_write_read( dat );
}

/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static inline BYTE rcvr_spi (void)
{
	return spi_write_read( 0xff );
}

/* replacement for the AVR-Macro */
static inline void rcvr_spi_m( BYTE *dst )
{
	*dst = rcvr_spi();
}



/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
BYTE wait_ready (void)
{
	BYTE res;


	Timer2 = 500*TIMER_EQV;	/* Wait for ready in timeout of 500ms */
	rcvr_spi();
	do
		res = rcvr_spi();
	while ((res != 0xFF) && Timer2--);

	return res;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void release_spi (void)
{
	DESELECT();
	rcvr_spi();
}



/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static
void power_on (void)
{
	init_SOCKPOWER();
	set_SOCKPOWER( TRUE );		/* Socket power ON */
	for (Timer1 = 30*TIMER_EQV; Timer1; Timer1--);	/* Wait for 30ms */
	init_SOCKWP_SOCKINS();      /* Init socket-switches */
	init_spi();
}

static
void power_off (void)
{
	SELECT();				/* Wait for card ready */
	wait_ready();
	close_spi();
	set_SOCKPOWER( FALSE );	/* Socket power OFF */
	Stat |= STA_NOINIT;		/* Set STA_NOINIT */
}

static
int chk_power(void)		/* Socket power state: 0=off, 1=on */
{
	return ( get_SOCKPOWER() ) ? 1 : 0;
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
BOOL rcvr_datablock (
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
	BYTE token;


	Timer1 = 1000000*TIMER_EQV;
	do {							/* Wait for data packet in timeout of 100ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && Timer1--);
	
	if(token != 0xFE) return FALSE;	/* If not valid data token, return with error */

	
#if USE_FIFO
	spi_rcvr_block( buff, btr );
#else
	do {							/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	} while (btr -= 4);
#endif /* USE_FIFO */
	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return TRUE;					/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
static
BOOL xmit_datablock (
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data/Stop token */
)
{
	BYTE resp, wc;


	if (wait_ready() != 0xFF) return FALSE;

	xmit_spi(token);					/* Xmit data token */
	if (token != 0xFD) {	/* Is data token */
		wc = 0;
#if USE_FIFO
		spi_xmit_block( buff );
#else
		do {							/* Xmit the 512 byte data block to MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);
#endif /* USE_DMA */
		xmit_spi(0xFF);					/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();				/* Receive data response */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return FALSE;
	}

	return TRUE;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;

#ifdef DEBUG_ESPARDINO_SDHC
	printf("CMD=[%02X %08X] =>",cmd,(unsigned int)arg);
#endif

	if (cmd & 0x80) {	/* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	DESELECT();
	SELECT();
	if (wait_ready() != 0xFF) return 0xFF;

	/* Send command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);
#ifdef DEBUG_ESPARDINO_SDHC	
	printf("%02x\r\n",res);
#endif
	return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{
	BYTE n, cmd, ty, ocr[4];

#if 0
	static BYTE pv;
	pv = get_SOCKWP() | get_SOCKINS();
	if (pv & SOCKINS) /* INS = H (Socket empty) */
		xprintf("not inserted\n");
	else {
		/* INS = L (Card inserted) */
		xprintf("inserted\n");
		if (pv & SOCKWP) /* WP is H (write protected) */
			xprintf("protected\n");
		else
			/* WP is L (write enabled) */
			xprintf("unprotected\n");
	}
#endif

	if (drv) return STA_NOINIT;			/* Supports only single drive */
	//if (Stat & STA_NODISK) return Stat;	/* No card in the socket */
	
	if (get_SOCKINS()) return STA_NODISK;

#ifdef DEBUG_ESPARDINO_SDHC
	printf("80 dummy cycles\r\n");
#endif	
	power_on();							/* Force socket power on */
	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		Timer1 = 1000*TIMER_EQV/10;						/* Initialization timeout of 1000 msec */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDHC */
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
				while (Timer1 && send_cmd(ACMD41, 1UL << 30)) Timer1--;	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (Timer1 && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
				}
			}
		} else {							/* SDSC or MMC */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDSC */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMC */
			}
			while (Timer1 && send_cmd(cmd, 0)) Timer1--;			/* Wait for leaving idle state */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	release_spi();

	if (ty) {			/* Initialization succeeded */
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
		if ( ty == CT_MMC ) {
			spiSetSpeed(SPI_SPEED_20MHz);
		} else {
			spiSetSpeed(SPI_SPEED_25MHz);
		}
	} else {			/* Initialization failed */
		power_off();
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive number (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	//int i,n;
	
	//memset(buff,0,512);
	
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
			count = 0;
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	release_spi();
	
	//printf(" disk_read sector=%d, count=%d\r\n\r\n",sector,count);
	/*n = 0;
	for (i=0;i<count*512;i++)
	{
		printf("%02X ",buff[i]);
		if (!((i+1)%32)) printf("\r\n\r\n");
		
	}
	printf("\r\n\r\n");
	
	*/

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (get_SOCKWP()) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	release_spi();

	return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY == 0 */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL != 0
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	WORD csize;

	//printf(" disk_ioctl(%d,%d);\r\n",drv,ctrl);

	if (drv) return RES_PARERR;

	res = RES_ERROR;

	if (ctrl == CTRL_POWER) {
		switch (*ptr) {
		case 0:		/* Sub control code == 0 (POWER_OFF) */
			if (chk_power())
				power_off();		/* Power off */
			res = RES_OK;
			break;
		case 1:		/* Sub control code == 1 (POWER_ON) */
			power_on();				/* Power on */
			res = RES_OK;
			break;
		case 2:		/* Sub control code == 2 (POWER_GET) */
			*(ptr+1) = (BYTE)chk_power();
			res = RES_OK;
			break;
		default :
			res = RES_PARERR;
		}
	}
	else {
		if (Stat & STA_NOINIT) return RES_NOTRDY;

		switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process */
			SELECT();
			if (wait_ready() == 0xFF)
				res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				} else {					/* SDC ver 1.XX or MMC*/
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			if (CardType & CT_SD2) {			/* SDC ver 2.00 */
				if (send_cmd(ACMD13, 0) == 0) {		/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) {				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {					/* SDC ver 1.XX or MMC */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
					if (CardType & CT_SD1) {			/* SDC ver 1.XX */
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMC */
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
			if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
			if (send_cmd(CMD10, 0) == 0		/* READ_CID */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 4; n; n--) *ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(ptr, 64))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
		}

		release_spi();
	}

	return res;
}
#endif /* _USE_IOCTL != 0 */



