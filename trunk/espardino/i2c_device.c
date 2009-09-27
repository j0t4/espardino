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


#include <LPC214x.h>                     /* LPC21xx definitions               */
#include "i2c_device.h"
#include <sysclocks.h>


#ifdef USE_I2C0
void irq_handler_I2C0 (void) __attribute__ ((interrupt("IRQ")));
i2c_state i2c0_state;
#endif

#ifdef USE_I2C1
void irq_handler_I2C1 (void) __attribute__ ((interrupt("IRQ")));
i2c_state i2c1_state;
#endif


void I2C_set_slave_addr(int mask, int addr)
{
	if (mask&I2C_0) I2C0_BASE[I2CADR] = addr;
	if (mask&I2C_1) I2C1_BASE[I2CADR] = addr;
}

void I2C_init(int mask, int speed_hz)
{
	int val = (VPB_get_speed()>>1)/speed_hz;
	
	if (val>65535) val = 65535;
	if (val<0) val = 0;

	if (mask&I2C_0)
	{
		 PINSEL0 |= 0x50; // P0.3 = SDA, P0.2 = SCL

		 I2C0_BASE[I2CADR]=DEFAULT_I2C0_ADDR;
		 I2C0_BASE[I2CCONSET] = I2C_I2EN | I2C_AA; // enable I2C hardwar and set AA (ack)
		 
		 /* setup  master  mode speed */
		 I2C0SCLH = val;	/* high scl time */
		 I2C0SCLL = val;    /* low scl time */

		/* Setup interrupt vector */

		 VICVectAddr3 = (unsigned long)irq_handler_I2C0; // set interrupt vector in 0
		 VICVectCntl3 = 0x20 | 9;                    // use it for I2C0 Interrupt
		 VICIntEnable = (1<<9);                  // Enable I2C0 Interrupt
	 
	}
	
	if (mask&I2C_1)
	{

		 PINSEL0 |= (0x3<<22) | (0x3<<28);
		 I2C1_BASE[I2CADR]=DEFAULT_I2C1_ADDR;
		 I2C1_BASE[I2CCONSET] = I2C_I2EN | I2C_AA; // enable I2C hardwar and set AA (ack)

		 /* setup  master  mode speed */
		 I2C1SCLH = val;
		 I2C1SCLL = val;

		 /* Setup interrupt vector */
		 VICVectAddr4 = (unsigned long)irq_handler_I2C1; // set interrupt vector in 0
		 VICVectCntl4 = 0x20 | 19;                    // use it for I2C1 Interrupt
		 VICIntEnable = (1<<19);                  // Enable I2C1 Interrupt
	}

}


void  i2c_irq_handler(volatile unsigned char *I2C,i2c_state *st)
{
	unsigned char hw_st;
	unsigned char dat;
	int i;


	dat =0;
	hw_st = I2C[I2CSTAT];
	

	// clear: interrupt flag,
	//        start generation
	//        ack enabled

	
	switch(hw_st)
	{
     /* BUS ERROR STATE */
	 	case 0x00:
			I2C[I2CCONSET] = I2C_STO| I2C_AA;
			break;

	 /* MASTER STATES */
	 	case 0x08:  // start condition generated 
		case 0x10: //  re-start condition generated
			
			dat = st->remote_i2c_addr;					

			if (st->operation == I2C_OP_READ)
			{
				dat = dat | 1;  // put the read flag on address byte
			}

			I2C[I2CDAT]    =   dat;	  // write the address of the device
			I2C[I2CCONSET] = I2C_AA;  // set AA
			I2C[I2CCONCLR] = I2C_STA; // clear SI (Interrupt Signal)

			break;

			
		case 0x18: 	/* slave device ACK the address */

			// first send the address inside device

			if (st->master_should_send_devaddr)   
			{
				dat = st->remote_dev_addr;
				st->master_should_send_devaddr = 0;
			} 
			else
			{
				if (st->write_len>0)	/* there is still data to be sent */
				{
					dat = *(st->write_pointer)++;
					st->write_len--;
				}	
			}			
			
			I2C[I2CDAT] 	= dat;
			I2C[I2CCONSET] 	= I2C_AA; // set AA
			break;


		case 0x20:  /* slave address not ACKd */
			st->result = I2C_RES_ADDR_NACK;
			st->write_len = 0;
			st->read_len = 0;
			st->operation = I2C_OP_NOP;
			I2C[I2CCONSET] =  I2C_STO | I2C_AA;
			break;


		case 0x28: 	   /* last data has been transmited (ACK Received)*/

			if (st->write_len>0)	/* there is still data to be sent */
			{
				st->write_len--;
				I2C[I2CDAT]    = *(st->write_pointer)++;
				I2C[I2CCONSET] = I2C_AA; // set AA
			}
			else
			{
				st->operation = I2C_OP_NOP;
				I2C[I2CCONSET] = I2C_STO|I2C_AA; /* stop */				
			}

			break;
	 
	 	case 0x30:    /* last data has been transmited (nACK Received)*/			

			st->operation = I2C_OP_NOP;		
			I2C[I2CCONSET] = I2C_STO|I2C_AA; /* stop */				

			break;


		case 0x38:    /* bus arbitration lost */
			I2C[I2CCONSET] = I2C_STA|I2C_AA; /* start when available */						
			break;

	 /* MASTER receiver */	 	

		case 0x40: /* Slave address + read transmited, (ACKd) */
			if (st->read_len ==1) 
			{
				I2C[I2CCONCLR] = I2C_AA;
			}
			else
			{
				I2C[I2CCONSET] = I2C_AA;
			}
			break;

		case 0x48: /* Slave address + read transmited, (not ACKd) */
			st->result = I2C_RES_ADDR_NACK;
			st->write_len = 0;
			st->read_len = 0;
			st->operation = I2C_OP_NOP;
			I2C[I2CCONSET] = I2C_STO | I2C_AA;
			break;

		case 0x50: /* Data has been received, ACK has been returned */
			*(st->read_pointer)++ = I2C[I2CDAT];
			st->read_len--;

			if (st->read_len<=1)
			{
				I2C[I2CCONCLR] = I2C_AA; // release bus, no ack
			}
			else
			{
				I2C[I2CCONSET] = I2C_AA;
			}
			
			break;

		case 0x58: /* data has been received, nACK returned */ 
			*(st->read_pointer)++ = I2C[I2CDAT];
			st->operation = I2C_OP_NOP;
			I2C[I2CCONSET]=I2C_STO|I2C_AA;
			break;


			

			


	 
	 /* SLAVE STATES */

		case 0x60: // own SLA+W received, Ack returned (slave receiver)
			st->slave_operation = I2C_OP_WRITE;
			st->i2c_addr_start = st->i2c_addr;
			
		case 0x68: // Addressed as slave
			I2C[I2CCONSET] = I2C_AA; // set AA, return ACK on first byte
			I2C[I2CCONCLR] = I2C_SI;
			st->slave_waiting_addr = 1;
			break;

		case 0x80: // Data received, ACK returned
			if (st->slave_waiting_addr) 
			{
				st->slave_waiting_addr = 0;
				st->i2c_addr = I2C[I2CDAT];
				st->i2c_addr_start = st->i2c_addr;
			}
			else
			{
				st->i2c_mmap_tmp[st->i2c_addr] = I2C[I2CDAT];
				// go to next addr
				st->i2c_addr++;
				// rollover the i2c_addr pointer
				if (st->i2c_addr>I2C_MMAP_SIZE) st->i2c_addr = 0;
			}
			I2C[I2CCONSET] = I2C_AA; // set AA, return ACK on first byte
			I2C[I2CCONCLR] = I2C_SI;
			break;

		case 0x88: // data received, NACK returned
			I2C[I2CCONSET] = I2C_AA; // set AA, switch to not addressed slave mode
			I2C[I2CCONCLR] = I2C_SI;			
			break;

		case 0xA0: // STOP or REP.START received while addressed as slave

			// if operation was write then flush the temporary buffer to the main
			// mmap one (this make write sequences atomic)
			if (st->slave_operation == I2C_OP_WRITE)
			{
				i = st->i2c_addr_start;
	
				while (i!=st->i2c_addr)
				{
					st->i2c_mmap[i] = st->i2c_mmap_tmp[i];
					i++;
					if (i>=I2C_MMAP_SIZE) i=0;
				}

				st->i2c_addr_start = i;
			}

			I2C[I2CCONSET] = I2C_AA; // set AA, switch to not addressed slave mode
			I2C[I2CCONCLR] = I2C_SI;						
			break;

		case 0xC0: // Data transmitted, NOT ACK received
		case 0xC8: // Last data transmitted, ACK received
			I2C[I2CCONSET] = I2C_AA; // set AA, switch to not addressed slave mode
			I2C[I2CCONCLR] = I2C_SI;
		break;
		case 0xA8: // own SLA+R received, Ack returned (slave transmitter)
				st->slave_operation = I2C_OP_READ;
		case 0xB8: // Data transmitted, ACK received

				I2C[I2CDAT]=st->i2c_mmap[st->i2c_addr];	 // return data from current i2c_addr
				// go to next addr
				st->i2c_addr++;
				// rollover the i2c_addr pointer
				if (st->i2c_addr>=I2C_MMAP_SIZE) st->i2c_addr = 0;
				I2C[I2CCONCLR] = I2C_SI;			
				break;

		default:
		I2C[I2CCONCLR] = I2C_SI;
		break;
	}

		I2C[I2CCONCLR] = I2C_SI;
//	sprintf(str,"%02x,",hw_st);
	//serial_device_puts0(str);


}

int i2c_start_write(volatile unsigned char *I2C,i2c_state *st,int i2c_addr, int dev_addr, unsigned char *p, int len)
{

	if (st->operation!=I2C_OP_NOP)
		return -1; 			// I2C OPERATION STILL RUNNING


	st->remote_i2c_addr = i2c_addr;
	if (dev_addr>=0)
	{
		st->remote_dev_addr = dev_addr;
		st->master_should_send_devaddr = 1;
	}
	else
	{
		st->master_should_send_devaddr = 0;
	}
	
	st->result = I2C_RES_0;	 
	

	st->write_len = len;
	st->write_pointer = p;

	st->operation = I2C_OP_WRITE;	

	I2C[I2CCONCLR] = I2C_SI; 	// clear interrupt flag
	I2C[I2CCONSET] = I2C_STA;	// signal start on bus	

	

	return 1;
}

int i2c_start_read(volatile unsigned char *I2C,i2c_state *st,int i2c_addr, int dev_addr, unsigned char *p, int len)
{

	if (st->operation!=I2C_OP_NOP)
		return -1; 			// I2C OPERATION STILL RUNNING
	

	if (dev_addr>=0)
	{
		st->remote_dev_addr = dev_addr;
		st->master_should_send_devaddr = 1;
	}
	else
	{
		st->master_should_send_devaddr = 0;
	}
	
	st->remote_i2c_addr = i2c_addr;
	st->result = I2C_RES_0;	 
	

	st->read_len = len;
	st->read_pointer = p;

	st->operation = I2C_OP_READ;
	
	I2C[I2CCONCLR] = I2C_SI; 	// clear interrupt flag
	I2C[I2CCONSET] = I2C_STA;	// signal start on bus	

	return 1;
}





#ifdef USE_I2C0
///////////////////////////////////////////////////////////////////////////////
//
// irq_handler_I2C0 (void)
// 
//

void irq_handler_I2C0 (void) 
{
	i2c_irq_handler(I2C0_BASE,&i2c0_state);	
	VICVectAddr = 0;       // Acknowledge Interrupt
}

int i2c_finished0()
{
	return i2c0_state.operation == I2C_OP_NOP;
}


int i2c_write_start0(int i2c_addr, int dev_addr, unsigned char *p, int len)
{
	return	i2c_start_write(I2C0_BASE,&i2c0_state,i2c_addr,dev_addr,p,len);
}

int i2c_read_start0(int i2c_addr, int dev_addr, unsigned char *p, int len)
{
	return	i2c_start_read(I2C0_BASE,&i2c0_state,i2c_addr,dev_addr,p,len);
}

unsigned char *i2c_get_mmap0(void)
{
	return i2c0_state.i2c_mmap;
}

int i2c_get_mmap_size0(void)
{
	return sizeof(i2c0_state.i2c_mmap);
}


#endif


#ifdef USE_I2C1
///////////////////////////////////////////////////////////////////////////////
//
// irq_handler_I2C1 (void)
// 
//

void irq_handler_I2C1 (void) {

	i2c_irq_handler(I2C1_BASE,&i2c1_state);
	VICVectAddr = 0;       // Acknowledge Interrupt

}

int i2c_finished1()
{
	return i2c1_state.operation == I2C_OP_NOP;
}


int i2c_write_start1(int i2c_addr, int dev_addr, unsigned char *p, int len)
{
	return	i2c_start_write(I2C1_BASE,&i2c1_state,i2c_addr,dev_addr,p,len);
}

int i2c_read_start1(int i2c_addr, int dev_addr, unsigned char *p, int len)
{
	return	i2c_start_read(I2C1_BASE,&i2c1_state,i2c_addr,dev_addr,p,len);
}

unsigned char *i2c_get_mmap1(void)
{
	return i2c1_state.i2c_mmap;
}

int i2c_get_mmap_size1(void)
{
	return sizeof(i2c1_state.i2c_mmap);
}

int I2C_write_nonblock(int mask, int i2c_addr, unsigned char *p,int len)
{
	if (mask&I2C_0) { i2c_write_start0(i2c_addr,-1,p,len); }
	if (mask&I2C_1) { i2c_write_start1(i2c_addr,-1,p,len); }
	
	return 0;
}

int I2C_read_nonblock(int mask, int i2c_addr, unsigned char *p, int len)
{

	if (mask&I2C_0) { i2c_read_start0(i2c_addr,-1,p,len); }
	if (mask&I2C_1) { i2c_read_start1(i2c_addr,-1,p,len); }
	
	return 0;
}


int I2C_wait(int mask)
{

	if (mask &I2C_0) { while (!i2c_finished0()) {} return i2c_finished0(); };
	if (mask &I2C_1) { while (!i2c_finished1()) {} return i2c_finished1(); };
	
	return 0;
	
}


int I2C_write(int mask, int i2c_addr, unsigned char *p,int len)
{

	I2C_write_nonblock(mask, i2c_addr, p,len);
	return I2C_wait(mask);
	
}

int I2C_read(int mask, int i2c_addr, unsigned char *p, int len)
{

	I2C_read_nonblock(mask,i2c_addr,p,len);
	return I2C_wait(mask);
}



#endif



