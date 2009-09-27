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



#ifndef __I2C_DEVICE_H
#define __I2C_DEVICE_H

#include "i2c_device_defs.h"

#define I2C_RES_0		  0
#define I2C_RES_ADDR_NACK 1


#define I2C_OP_NOP	   0
#define I2C_OP_READ    1
#define I2C_OP_WRITE   2

#define USE_I2C0	0x01
#define USE_I2C1	0x02

#define I2C_0	0x01
#define I2C_1	0x02

#define I2C_MMAP_SIZE 0x20

#define DEFAULT_I2C0_ADDR 0x30
#define DEFAULT_I2C1_ADDR 0x40



typedef struct {
			volatile int operation;
			
			/* I2C MASTER STATE INFORMATION */
			unsigned char remote_i2c_addr; // in the buffer
			unsigned char remote_dev_addr; // inside device

			int result;
			
			int master_should_send_devaddr;

			unsigned char *read_pointer;
			int 		  read_len;


			unsigned char *write_pointer;
			int 		  write_len;


			
			/* I2C SLAVE STATE INFORMATION */
			int slave_waiting_addr;
			unsigned char i2c_mmap[I2C_MMAP_SIZE];
			unsigned char i2c_mmap_tmp[I2C_MMAP_SIZE];

			int i2c_addr_start;
			int slave_operation;
			int i2c_addr;
} i2c_state;

#ifdef __cplusplus
extern "C" {
#endif

void i2c_irq_handler(volatile unsigned char *I2C,i2c_state *st);
void I2C_init(int mask, int speed_khz);
void I2C_set_slave_addr(int mask, int addr);

int I2C_read(int mask, int i2c_addr, unsigned char *p, int len);
int I2C_write(int mask, int i2c_addr, unsigned char *p, int len);

int I2C_read_nonblock(int mask, int i2c_addr, unsigned char *p, int len);
int I2C_write_nonblock(int mask, int i2c_addr, unsigned char *p, int len);


int I2C_wait(int mask);

#ifdef USE_I2C0
int i2c_finished0(void);
int i2c_write_start0(int i2c_addr, int dev_addr, unsigned char *p, int len);
int i2c_read_start0(int i2c_addr, int dev_addr, unsigned char *p, int len);
unsigned char *i2c_get_mmap0(void);
int i2c_get_mmap_size0(void);
#endif


#ifdef USE_I2C1
int i2c_finished1(void);
int i2c_write_start1(int i2c_addr, int dev_addr, unsigned char *p, int len);
int i2c_read_start1(int i2c_addr, int dev_addr, unsigned char *p, int len);
unsigned char *i2c_get_mmap1(void);
int i2c_get_mmap_size1(void);

#endif


#ifdef __cplusplus
}
#endif


#endif
