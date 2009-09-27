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



#ifndef __I2C_DEVICE_DEFS_H_
#define __I2C_DEVICE_DEFS_H


// UART addresses
#define I2C0_BASE ((volatile unsigned char *) 0xE001C000)
#define I2C1_BASE ((volatile unsigned char *) 0xE005C000)

/* I2C Interface 0 */

#define I2CCONSET     0x00
#define I2CSTAT       0x04
#define I2CDAT        0x08
#define I2CADR        0x0C
#define I2CSCLH       0x10	USE AS SHORT
#define I2CSCLL       0x14	USE AS SHORT
#define I2CCONCLR     0x18



/* I2CCON */

#define I2C_AA	(1<<2)		/* acknowledge slave address */
#define I2C_SI  (1<<3)		/* Interrupt flag */						    
#define I2C_STO (1<<4)      /* STOP */
#define I2C_STA (1<<5)      /* START */
#define I2C_I2EN (1<<6)		/* ENABLE */

/* I2CADR */ 

#define I2C_GC	1   /* General call address recognition */





#endif
