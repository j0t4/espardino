/*****************************************************************************
//
// File Name    : 'enc28j60.c'
// Title        : Driver for the Microchip ENC28J60 Ethernet controller
// Author       : Bernard Debbasch - Copyright (C) 2005
// Created      : 2005.08.17
// Revised      : 2009.10.15 NBEE Embedded Systems S.L.
// Version      : 1.0
// Target MCU   : Philips LPC213x/214x series
// Editor Tabs  : 2
//
//
//
// Copyright (C) 2005, Bernard Debbasch
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote
//    products derived from this software without specific prior
//    written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
 *******************************************************************************/

#include <stdio.h>
#include <string.h>


#ifdef USE_FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#else
#define portTICK_RATE_MS 1
#define vTaskDelay(x) delay_ms(x)
#define xTaskGetTickCount(x) t1_get_ms(x)
#endif


#include "../uip/uip.h"
#include "../uip/uip_arp.h"
#include "../uip/uipopt.h"
#include "enc28j60.h"

#include <micro214x.h>
#include <arm_irqs.h>
#include <netif.h>

//
//  RX & TX memory organization
//
#define MAXFRAMESIZE    1518
#define RAMSIZE       0x2000
#define TXSTART       0x0000
#define TXEND         0x0fff
#define RXSTART       0x1000
#define RXEND         0x1fff
#define RXSIZE        (RXSTOP - RXSTART + 1)

//
//  Chose one or the other
//
#undef  HALF_DUPLEX
#define FULL_DUPLEX

#ifdef HALF_DUPLEX
#ifdef FULL_DUPLEX
#error "DUPLEX==> CHOOSE ONE OR THE OTHER"
#endif
#endif

#define USE_INTERRUPTS

//
//  Local Prototypes
//
static u8_t encReadEthReg (u8_t address);
static u8_t encReadMacReg (u8_t address);
static void encWriteReg (u8_t address, u8_t data);
static void encWriteReg16 (u8_t address, u16_t data);
static void encBFSReg (u8_t address, u8_t data);
static void encBFCReg (u8_t address, u8_t data);
static void SendSystemCommand (void);
static void encBankSelect (u8_t bank);
static void encWritePHYReg (u8_t address, u16_t data);
static u16_t encReadPHYReg (u8_t address);
static void encMACwrite (u8_t data);
static void encMACwriteBulk (u8_t *buffer, u16_t length);
static u8_t encMACread (void);
static void encMACreadBulk (u8_t *buffer, u16_t length);

//
//  Global variable
//
u16_t ethRxPointer = 0;

#ifdef USE_FREERTOS
xSemaphoreHandle xENC28J60Semaphore = NULL;
#endif


void enc28j60_eint0ISR (void);

// SSPCR0  Bit-Definitions
#define CPOL    6
#define CPHA    7
// SSPCR1  Bit-Defintions
#define SSE     1
#define MS      2
#define SCR     8
// SSPSR  Bit-Definitions
#define TNF     1
#define RNE     2
#define BSY		4

#define SPI_PRESCALE_REG  SSPCPSR

#ifndef U8
typedef unsigned char U8;
#endif


unsigned char enc28j60_mac[]={0xDE,0xAD,0xBE,0xEF,0x12,0x13};

unsigned char *enc28j60_GetMacAddr(void)
{

	return enc28j60_mac;
}

void enc28j60_SetMacAddr(unsigned char *mac)
{
	memcpy(enc28j60_mac,mac,6);
}

int enc28j60_IsLinked()
{
	return (encReadPHYReg (PHSTAT1) & PHSTAT1_LLSTAT);
}

void enc28j60_Relink()
{
	return;
}

void enc28j60_Reset()
{
	enc28j60Deinit();
	enc28j60Init();
}

void enc28j60_Process()
{

}


netif enc28j60_driver_ops =
{
	&enc28j60_GetMacAddr,   // get_mac_address
	&enc28j60_SetMacAddr,   // set_mac_address
	&enc28j60_IsLinked,     // is_linked
	&enc28j60_Relink,       // relink
	&enc28j60_Reset,		  // reset
	&enc28j60WaitForData,     // wait_data
	&enc28j60_Process,   // process
	&enc28j60Send,       // tx_data
	&enc28j60Receive        // rx_data
};


netif *enc28j60_GetNetIf()
{
	return &enc28j60_driver_ops;
}


void enc28j60_spiInit(void);

void enc28j60_spiInit()
{

	SSPCR1 &= ~(1<<SSE);

	SSPCR0 = ((8-1)<<0) | (0<<CPOL) | (0x2<<SCR); //  (0xff<<SCR);  // 8 bit transfers

	PINSEL1 = (PINSEL1 & (~(3<<2)))|(2<<2); // SCK1
	PINSEL1 = (PINSEL1 & (~(3<<4)))|(2<<4); // MISO1
	PINSEL1 = (PINSEL1 & (~(3<<6)))|(2<<6); // MOSI1

	SPI_PRESCALE_REG = 2; //4 , 5

	SSPCR1 = (1<<SSE); // Enable SSP, as Master



}


U8 enc28j60_spiPut (U8 valueIn);

U8 enc28j60_spiPut (U8 valueIn)
{
	U8 val;
	while( !(SSPSR & (1<<TNF)) ) ; // while Transmit fifo full
	SSPDR = valueIn;
	while( !(SSPSR & (1<<RNE)) ) ; // while receive fifo empty

	val = SSPDR;

	return val;
}


/******************************************************************************
 * Function:        void EthernetInit (void)
 * PreCondition:    SPI must be configured
 * Input:           None
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Initialize the ENC28J60
 * Note:            None
 *****************************************************************************/
int enc28j60Init (void)
{
  volatile portTickType xTicks;

  //
  //  It'd probably be better if the EINT initialization code took a pointer to
  //  the semaphore, rather than just "knowing" about it, but it's hard to make
  //  that sort of thing generic, since an interrupt routine may not really
  //  want to use a semaphore, and instead use a queue or somesuch.
  //

#ifdef USE_FREERTOS
  if (!xENC28J60Semaphore)
  {
    vSemaphoreCreateBinary (xENC28J60Semaphore);

    if (!xENC28J60Semaphore)
      return 0;
  }

  xSemaphoreTake (xENC28J60Semaphore, 0);
#endif

  ENC28J60_Reset ();

  //
  //  Disable UART1 so we can use the RXD pin as EINT3
  //

  // EINT0 init

  PINSEL1= (PINSEL1 & (~3)) | 1; // EINT0 enabled on P0.16
  EXTMODE = EXTMODE | 1;   	      // EINT0 is edge sensitive
  EXTPOLAR = EXTPOLAR & (~1);    // EINT0 on falling edge

/*
  VICIntSelect &= ~(1<<14);    // enable as IRQ (not as FIQ)
  VICVectAddr6 = (unsigned long) enc28j60_eint0ISR;
  VICVectCntl6 = 0x20 | 14;
  VICIntEnable = (1<<14);
*/
	VIC_setup_irq(14,  enc28j60_eint0ISR);

  // SPI1 init
  enc28j60_spiInit();



  //
  //  Now hold part in reset for 100ms
  //
  ENC28J60_Deselect ();
  ENC28J60_Reset ();

  vTaskDelay (100 / portTICK_RATE_MS);
  ENC28J60_Unreset ();
  vTaskDelay (100 / portTICK_RATE_MS);

  //
  //  Give the part 1 second for the PHY to become ready (CLKRDY == 1).  If it
  //  doesn't, return an error to the user.
  //
  //  Note that we also check that bit 3 is 0.  The data sheet says this is
  //  unimplemented and will return 0.  We use this as a sanity check for the
  //  ENC28J60 actually being present, because the MISO line typically floats
  //  high.  If we only checked the CLKRDY, it will likely return 1 for when no
  //  ENC28J60 is present.
  //
  xTicks = xTaskGetTickCount ();

  while (((encReadEthReg (ESTAT) & (ESTAT_UNIMP | ESTAT_CLKRDY)) != ESTAT_CLKRDY))
    if ((xTaskGetTickCount () - xTicks) > (1000 / portTICK_RATE_MS))
      return 0;

  //
  //  Send a Soft Reset to the chip
  //
  SendSystemCommand ();
  vTaskDelay (20 / portTICK_RATE_MS);

  encBankSelect (BANK0);

  ethRxPointer = RXSTART;             // Initialize the local RX pointer...
  encWriteReg16 (ERXSTL, RXSTART);    // ...and configure the base address */
  encWriteReg16 (ERXRDPTL, RXSTART);  // Read pointer is also at the base address for now
  encWriteReg16 (ERXNDL, RXEND);      // Configure the H/W with the end of memory location
  encWriteReg16 (ETXSTL, TXSTART);    // Configure the H/W with the TX base address

  encBankSelect (BANK1);
  encWriteReg16 (ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

  encBankSelect (BANK2);
  encWriteReg (MACON2, 0);  // Remove all reset conditions

#ifdef HALF_DUPLEX
  encWriteReg (MACON1, MACON1_MARXEN);                    // Enable RX
  encWriteReg (MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN);  // Automatic padding, CRC generation
#else
  encWriteReg (MACON1, MACON1_TXPAUS | MACON1_RXPAUS | MACON1_MARXEN);    // Enable TX pause, RX pause and RX
  encWriteReg (MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FULDPX);  // Automatic padding, CRC generation, Full-Duplex
#endif

  encWriteReg16 (MAMXFLL, MAXFRAMESIZE);  // Set the maximum packet size which the controller will accept

  encWriteReg16 (MAIPGL, 0x0c12);         // Set inter-packet gap to 9.6us

  encWriteReg (MABBIPG, 0x12);            // Set back to back inter-packet gap

  encBankSelect (BANK3);
  encWriteReg (MAADR1, enc28j60_mac [0]);
  encWriteReg (MAADR2, enc28j60_mac [1]);
  encWriteReg (MAADR3, enc28j60_mac [2]);
  encWriteReg (MAADR4, enc28j60_mac [3]);
  encWriteReg (MAADR5, enc28j60_mac [4]);
  encWriteReg (MAADR6, enc28j60_mac [5]);

#ifdef HALF_DUPLEX
  encWritePHYReg (PHCON1, 0);
#else
  encWritePHYReg (PHCON1, PHCON1_PDPXMD);
#endif

  encWritePHYReg (PHCON2, PHCON2_HDLDIS);   // Disable half duplex loopback in PHY.

  encReadPHYReg (PHID1);          // Read a PHY register
  encBFSReg (ECON1, ECON1_RXEN);  // Enable packet reception

  return 1;
}

void enc28j60Deinit (void)
{
  ENC28J60_Reset ();

}

//
//
//
int	 enc28j60Send (u8_t *uip_buf, int uip_len)
{
  u16_t length;
  u16_t value;

  length = uip_len;           // Save length for later

  encBankSelect (BANK0);
  encWriteReg16 (EWRPTL, TXSTART);  // Configure the H/W with the TX Start and End addresses
  encMACwrite (0x00);               // Write the per packet option byte we use the default values set in chip earlier

  encMACwriteBulk (&uip_buf [0], 54);   // Send 40 + 14 = 54 bytes of header

  //
  // Send the rest of the packet, the application data
  //
  if (uip_len > 54)
  {
    uip_len -= 54;
    encMACwriteBulk (&uip_buf[54], uip_len);
  }

  //
  //  Configure the H/W with the TX Start and End addresses
  //
  encWriteReg16 (ETXSTL, TXSTART);  // Configure the H/W with the TX base address
  encWriteReg16 (ETXNDL, (TXSTART + length));
  value  = encReadEthReg (ETXNDL);
  value += encReadEthReg (ETXNDH) << 8;

  //
  //  Enable packet Transmission
  //
  encBFCReg (EIR, EIR_TXIF);
  encBFSReg (ECON1, ECON1_TXRST);         // Errata for B5
  encBFCReg (ECON1, ECON1_TXRST);         // Errata for B5
  encBFCReg (EIR, EIR_TXERIF | EIR_TXIF); // Errata for B5

  encBFSReg (ECON1, ECON1_TXRTS);

  return uip_len;
}

//
//
//
int enc28j60Receive (u8_t *uip_buf,int max_len)
{
  u16_t len = 0;
  u16_t u;

#ifndef USE_INTERRUPTS
  //
  //  Check if at least one packet has been received and is waiting
  //
  if ((encReadEthReg (EIR) & EIR_PKTIF) == 0)
    return 0;
#else
  //
  //  Sanity check that at least one full packet is present (spurious interrupts?)
  //
  encBankSelect (BANK1);

  if (!encReadEthReg (EPKTCNT))
    return 0;
#endif

  //
  //  Set read Pointer
  //
  encBankSelect (BANK0);
  encWriteReg16 (ERDPTL, ethRxPointer);

  ethRxPointer = encMACread ();
  ethRxPointer += encMACread () << 8;

  //
  //  Sanity check
  //
  if (ethRxPointer > RXEND)
  {
    enc28j60Init ();
    return 0;
  }

  len = encMACread ();
  len += encMACread () << 8;

  //
  //  Read 2 more statuses that we ignore (why?)
  //
  encMACread ();
  encMACread ();

  //
  //  If the frame is too big to handle, throw it away
  //
  if (len > max_len)
  {
    for (u = 0; u < len; u++)
      encMACread ();

    return 0;
  }

  //
  //  Read the whole frame
  //
  encMACreadBulk (&uip_buf [0], len);

  //
  //  Clean up for next packet.  Set the read pointer to the start of RX packet to free up space used by the current frame
  //
  encBankSelect (BANK0);
  encWriteReg16 (ERXRDPTL, ethRxPointer);

  //
  //  Decrement EPKTCNT
  //
  encBFSReg (ECON2, ECON2_PKTDEC);

  //
  //  Return the length - the 4 bytes of CRC (why?)
  //
  return (len - 4);
}

//
//
//
extern int enc28j60_had_interrupt;

int enc28j60WaitForData (int  delay)
{
#ifdef USE_FREERTOS
	portBASE_TYPE semStat;
#endif

#ifdef USE_INTERRUPTS
  encBFCReg (EIR, EIR_PKTIF);
  encWriteReg (EIE, EIE_INTIE | EIE_PKTIE);
#endif
#ifdef USE_FREERTOS
  if ((semStat = xSemaphoreTake (xENC28J60Semaphore, delay)) == pdPASS)
  {

#ifdef USE_INTERRUPTS
    encBFCReg (EIE, EIE_INTIE);
#endif
  }

  return semStat;

#else
  if (enc28j60_had_interrupt)
  {
	  encBFCReg (EIE, EIE_INTIE);
	  return 1;
  }
  return 0;

#endif




}

/******************************************************************************
 * Function:        void SendSystemCommand (void)
 * PreCondition:    None
 * Input:           None
 * Output:          None
 * Side Effects:    All values are lost
 *
 * Overview:        Send the SC (Reset) command to the device
 * Note:            None
 *****************************************************************************/
static void SendSystemCommand (void)
{
  ENC28J60_Select ();

  enc28j60_spiPut (SC);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        u8_t encReadEthReg (u8_t address)
 * PreCondition:    None
 * Input:           address to read
 * Output:          value read from the register
 * Side Effects:    None
 *
 * Overview:        read the value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static u8_t encReadEthReg (u8_t address)
{
  u8_t value;

  //
  //  Tcss (/CS setup time) is 50ns, Tcsh (/CS hold time) is 10ns ETH registers
  //  and memory buffer, 210ns MAC and MII registers.
  //
  ENC28J60_Select ();

  enc28j60_spiPut (RCR | address);
  value = enc28j60_spiPut (0x00);

  ENC28J60_Deselect ();

  return value;
}

/******************************************************************************
 * Function:        u8_t encReadMacReg (u8_t address)
 * PreCondition:    None
 * Input:           address to read
 * Output:          value read from the register
 * Side Effects:    None
 *
 * Overview:        read the value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static u8_t encReadMacReg (u8_t address)
{
  u8_t value;

  ENC28J60_Select ();

  enc28j60_spiPut (RCR | address);
  enc28j60_spiPut (0x00);             /* Send a dummy byte */
  value = enc28j60_spiPut (0x00);

  ENC28J60_Deselect ();

  return value;
}

/******************************************************************************
 * Function:        void encWriteReg (u8_t address, u8_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Send value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static void encWriteReg (u8_t address, u8_t data)
{
  ENC28J60_Select ();

  enc28j60_spiPut (WCR | address);
  enc28j60_spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encBFSReg (u8_t address, u8_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data field
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Set the data field in the address register
 * Note:            None
 *****************************************************************************/
static void encBFSReg (u8_t address, u8_t data)
{
  ENC28J60_Select ();

  enc28j60_spiPut (BFS | address);
  enc28j60_spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encBFCReg (u8_t address, u8_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data fiels
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Clear the data field in the address register
 * Note:            None
 *****************************************************************************/
static void encBFCReg (u8_t address, u8_t data)
{
  ENC28J60_Select ();

  enc28j60_spiPut (BFC | address);
  enc28j60_spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encWriteReg16 (u8_t address, u16_t data)
 * PreCondition:    Bank must point ot the right bank
 * Input:           address and data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Send value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static void encWriteReg16 (u8_t address, u16_t data)
{
  encWriteReg (address,   (u8_t) (data & 0xff));
  encWriteReg (address+1, (u8_t) (data >> 8));
}

/******************************************************************************
 * Function:        void encWritePHYReg (u8_t address, u16_t data)
 * PreCondition:    None
 * Input:           address and data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Send value at the address over the SPI bus
 * Note:            None
 *****************************************************************************/
static void encWritePHYReg (u8_t address, u16_t data)
{
  encBankSelect (BANK2);

  encWriteReg (MIREGADR, address);
  encWriteReg16 (MIWRL, data);

  encBankSelect (BANK3);

  while ((encReadMacReg (MISTAT) & MISTAT_BUSY))
    ;
}

/******************************************************************************
 * Function:        u16_t encReadPHYReg (u8_t address)
 * PreCondition:    None
 * Input:           address to read from
 * Output:          data from a PHY register
 * Side Effects:    None
 *
 * Overview:        Read a value from a PHY register
 * Note:            None
 *****************************************************************************/
static u16_t encReadPHYReg (u8_t address)
{
  u16_t value;

  encBankSelect (BANK2);

  encWriteReg (MIREGADR, address);
  encWriteReg (MICMD, MICMD_MIIRD);

  encBankSelect (BANK3);

  while ((( encReadMacReg (MISTAT)) & MISTAT_BUSY))
    ;

  encBankSelect (BANK2);

  encWriteReg (MICMD, 0);

  value  = encReadMacReg (MIRDH) << 8;
  value += encReadMacReg (MIRDL);

  return value;
}

/******************************************************************************
 * Function:        void encBankSelect (u8_t bank)
 * PreCondition:    None
 * Input:           bank (0, 1, 2 or 3)
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Set the bank select
 * Note:            None
 *****************************************************************************/
static void  encBankSelect (u8_t bank)
{
  encBFCReg (ECON1, ECON1_BSEL1 | ECON1_BSEL0);
  encBFSReg (ECON1, bank);
}

/******************************************************************************
 * Function:        void encMACwrite (u8_t data)
 * PreCondition:    EWRPT must point to the location to be written to
 * Input:           data to write
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Write a byte in the MAC memory with the intention
 *                  of sending a frame later
 * Note:            None
 *****************************************************************************/
static void encMACwrite (u8_t data)
{
  ENC28J60_Select ();

  enc28j60_spiPut (WBM);
  enc28j60_spiPut (data);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        void encMACwriteBulk (u8_t *buffer, u16_t length)
 * PreCondition:    EWRPT must point to the location to be written to
 * Input:           data to write and length of data
 * Output:          None
 * Side Effects:    None
 *
 * Overview:        Multi Write in the MAC memory with the intention
 *                  of sending a frame later
 * Note:            None
 *****************************************************************************/
static void encMACwriteBulk (u8_t *buffer, u16_t length)
{
  ENC28J60_Select ();

  enc28j60_spiPut (WBM);

  while (length--)
    enc28j60_spiPut (*buffer++);

  ENC28J60_Deselect ();
}

/******************************************************************************
 * Function:        u8_t data encMACread (void)
 * PreCondition:    ERDPT must point to the location to read from
 * Input:           None
 * Output:          Data read
 * Side Effects:    None
 *
 * Overview:        Read a byte from the MAC memory
 * Note:            None
 *****************************************************************************/
static u8_t encMACread (void)
{
  u8_t value;

  ENC28J60_Select ();

  enc28j60_spiPut (RBM);
  value = enc28j60_spiPut (0x00);

  ENC28J60_Deselect ();

  return value;
}

/******************************************************************************
 * Function:        encMACreadBulk (u8_t *buffer, u16_t length)
 * PreCondition:    ERDPT must point to the location to read from
 * Input:           Buffer to put the data and length of data to read
 * Output:          Data read in the buffer
 * Side Effects:    None
 *
 * Overview:        Read multiple bytes from the MAC memory
 * Note:            None
 *****************************************************************************/
static void encMACreadBulk (u8_t *buffer, u16_t length)
{
  ENC28J60_Select ();

  enc28j60_spiPut (RBM);

  while (length--)
    *buffer++ = enc28j60_spiPut  (0x00);

  ENC28J60_Deselect ();
}
