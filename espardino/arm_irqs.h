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


#ifndef __ARM_IRQS_H
#define __ARM_IRQS_H

#define CPSR_IRQ_MASK 0x00000080
#define CPSR_FIQ_MASK 0x00000040
#define CPSR_INT_MASK (CPSR_IRQ_MASK | CPSR_FIQ_MASK)


#ifdef __cplusplus
extern "C" {
#endif

unsigned IRQ_disable(void);
unsigned IRQ_restore(unsigned old_cpsr);
unsigned IRQ_enable(void);
unsigned FIQ_disable(void);
unsigned FIQ_restore(unsigned old_cpsr);
unsigned FIQ_enable(void);
unsigned INT_disable(void);
unsigned INT_restore(unsigned old_cpsr);
unsigned INT_enable(void);
int VIC_setup_irq(int intnum,  void *IRQHandler);
int VIC_setup_fiq(int intnum,  void *IRQHandler);


/* interrupt numbers */
#define WDT_INT 		0
#define TIMER0_INT  4
#define TIMER1_INT  5
#define UART0_INT   6
#define UART1_INT   7
#define PWM0_INT    8
#define I2C0_INT    9
#define SPI0_INT    10
#define SPI1_INT    11
#define PLL_INT     12
#define RTC_INT     13
#define EINT0_INT   14
#define EINT1_INT   15
#define EINT2_INT   16
#define EINT3_INT   17
#define ADC0_INT    18
#define I2C1_INT    19
#define BOD_INT     20
#define ADC1_INT    21
#define USB_INT     22


#ifdef __cplusplus
}
#endif


#endif