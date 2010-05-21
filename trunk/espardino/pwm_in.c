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
#include <arm_irqs.h>


#define CAP0_0  (1<<0)
#define CAP0_1  (1<<1)
#define CAP0_2  (1<<2)
#define CAP0_3  (1<<3)


#define CAP1_0  (1<<4)
#define CAP1_1  (1<<5)
#define CAP1_2  (1<<6)
#define CAP1_3  (1<<7)



#define CAP0RE (1<<0)
#define CAP0FE (1<<1)
#define CAP0I  (1<<2)

#define CAP1RE  (1<<3)
#define CAP1FE  (1<<4)
#define CAP1I   (1<<5)

#define CAP2RE  (1<<6)
#define CAP2FE  (1<<7)
#define CAP2I   (1<<8)

#define CAP3RE  (1<<9)
#define CAP3FE  (1<<10)
#define CAP3I   (1<<11)


volatile unsigned int CCP1_start[4];
volatile unsigned int CCP1_end[4];
volatile unsigned int CCP1_length[4];
volatile unsigned int CCP1_count[4];



//pragma GCC optimize "O0"
static void CCP_IRQ_T1(void) __attribute__ ((interrupt("IRQ")));// __attribute__((optimize("O0")));
static void CCP_IRQ_T1(void) 
{
	int clear_ints = 0;

	if (T1IR & (1<<4))
	{
		if (T1CCR & CAP0RE) /* We were waiting for rising edge */
		{
			CCP1_start[0] = T1CR0;
			T1CCR = (T1CCR & (~CAP0RE))|CAP0FE|CAP0I; /* wait now for falling edge */
		} 
		else /* we were waiting for falling edge */
		{
			CCP1_end[0] = T1CR0;
			CCP1_length[0] = CCP1_end[0]-CCP1_start[0];
			CCP1_count[0]++;
			T1CCR = (T1CCR & (~CAP0FE))|CAP0RE|CAP0I;
		}
		clear_ints |= (1<<4);
	}
	
	if (T1IR & (1<<5))
	{
		if (T1CCR & CAP1RE) /* We were waiting for rising edge */
		{
			CCP1_start[1] = T1CR1;
			T1CCR = (T1CCR & (~CAP1RE))|CAP1FE|CAP1I; /* wait now for falling edge */
		} 
		else /* we were waiting for falling edge */
		{
			CCP1_end[1] = T1CR1;
			CCP1_length[1] = CCP1_end[1]-CCP1_start[1];
			CCP1_count[1]++;
			T1CCR = (T1CCR & (~CAP1FE))|CAP1RE|CAP1I;
		}
		clear_ints |= (1<<5);
	}
	
	if (T1IR & (1<<6))
	{
		if (T1CCR & CAP2RE) /* We were waiting for rising edge */
		{
			CCP1_start[2] = T1CR2;
			T1CCR = (T1CCR & (~CAP2RE))|CAP2FE|CAP2I; /* wait now for falling edge */
		} 
		else /* we were waiting for falling edge */
		{
			CCP1_end[2] = T1CR2;
			CCP1_length[2] = CCP1_end[2]-CCP1_start[2];
			CCP1_count[2]++;
			T1CCR = (T1CCR & (~CAP2FE))|CAP2RE|CAP2I;
		}
		clear_ints |= (1<<6);
	}
	
	
	if (T1IR & (1<<7))
	{
		if (T1CCR & CAP3RE) /* We were waiting for rising edge */
		{
			CCP1_start[3] = T1CR3;
			T1CCR = (T1CCR & (~CAP3RE))|CAP3FE|CAP3I; /* wait now for falling edge */
		} 
		else /* we were waiting for falling edge */
		{
			CCP1_end[3] = T1CR3;
			CCP1_length[3] = CCP1_end[3]-CCP1_start[3];
			CCP1_count[3]++;
			T1CCR = (T1CCR & (~CAP3FE))|CAP3RE|CAP3I;
		}
		clear_ints |= (1<<6);
	}
	
	
	
	T1IR = T1IR;
	VICVectAddr = 0;
}
//#pragma GCC optimize "Os"

void CCP1_IRQ_Init()
{
	/*
	VICIntSelect &= ~(1<<5);    
	VICVectAddr1 = (unsigned long) CCP_IRQ_T1;
	VICVectCntl1 = 0x20 | 5;
	VICIntEnable = (1<<5);
	*/
	VIC_setup_irq(5, CCP_IRQ_T1);
}



void PWM_in_init(int mask)
{

	if (mask&CAP1_0)
	{
		PINSEL0 = (PINSEL0 & ~(3<<20)) | (2<<20); // CAP1.0 on P0.10
		T1CCR |= CAP0RE|CAP0I;
	}
		
	if (mask&CAP1_1)
	{
		PINSEL0 = (PINSEL0 & ~(3<<22)) | (2<<22); // CAP1.1 on P0.11
		T1CCR |= CAP1RE|CAP1I;
		
	}
	
	if (mask&CAP1_2)
	{
		PINSEL1 = (PINSEL1 & ~(3<<6)) | (3<<6); // CAP1.2 on P0.19
		T1CCR |= CAP2RE|CAP2I;
	}
	
	if (mask&CAP1_3)
	{
		PINSEL1 = (PINSEL1 & ~(3<<4)) | (1<<4); // CAP1.3 on P0.18
		T1CCR |= CAP3RE|CAP3I;
	}


	if (mask&(CAP1_0|CAP1_1|CAP1_2|CAP1_3))
	{
		T1MCR = 0;
		T1TCR = 1; //timer 1 enable
		
		CCP1_IRQ_Init();
	}

}

int PWM_in_get_count(int port)
{
	switch (port)
	{
		case CAP1_0: return CCP1_count[0];
		case CAP1_1: return CCP1_count[1];
		case CAP1_2: return CCP1_count[2];
		case CAP1_3: return CCP1_count[3];
		default:
			return -1;
	}
}

int PWM_int_get_ticks(int port)
{
	switch (port)
	{
		case CAP1_0: return CCP1_length[0];
		case CAP1_1: return CCP1_length[1];
		case CAP1_2: return CCP1_length[2];
		case CAP1_3: return CCP1_length[3];
		default:
			return -1;
	}

}

void CCP_init(int mask)
{
	PWM_in_init(mask);
}

