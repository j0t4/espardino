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



#include <micro214x.h>
#include <LPC214x.h>
#include <ticker.h>

void irq_handler_TIMER0 (void) __attribute__ ((interrupt("IRQ")));
void irq_handler_TIMER1 (void) __attribute__ ((interrupt("IRQ")));

void irq_handler_TIMER0()
{
	if (Ticker::m_functionT0) Ticker::m_functionT0();
	
	T0IR        = 1; /* clear timer 0 interrupt */                 
	VICVectAddr = 0; /* acknowledge to the VIC controller */
}


void irq_handler_TIMER1()
{
	if (Ticker::m_functionT1) Ticker::m_functionT1();

	T1IR        = 1; /* clear timer 0 interrupt */                         
	VICVectAddr = 0; /* acknowledge to the VIC controller */

}


ptVoidFunction Ticker::m_functionT0 = 0;
ptVoidFunction Ticker::m_functionT1 = 0;


Ticker::Ticker(int timer)
{
	m_timer = timer;
	
  if (m_timer==TIMER0)
  	VIC_setup_irq(4,(void *)irq_handler_TIMER0);
  else
    VIC_setup_irq(5,(void *)irq_handler_TIMER1);
}

Ticker::~Ticker()
{
	stop();
}

void Ticker::attach(ptVoidFunction function)
{
	switch(m_timer)
	{
		case TIMER1: m_functionT1 = function;
								 break;
								 
		case TIMER0: 
			default:m_functionT0 = function;
	}
}

void Ticker::attach(ptVoidFunction function,float period)
{
		attach(function);
		setPeriod(period);
}

void Ticker::setFrequency(int freq)
{
	    
	if (m_timer==TIMER0)
	{
		T0MR0 = (VPB_get_speed()/freq) - 1;                   
		T0MCR = 3;            
	} else
	{
		T1MR0 = (VPB_get_speed()/freq) - 1;                   
		T1MCR = 3;            
	}
		                           
}

void Ticker::setPeriod(float period)
{
	setFrequency(1.0/period);
}


void Ticker::start()
{
	switch(m_timer)
	{
		case TIMER0: T0TCR = 1;   break;
		case TIMER1: T1TCR = 1;   break;
	}
}

void Ticker::stop()
{
	switch(m_timer)
	{
		case TIMER0: T0TCR = 0;   break;
		case TIMER1: T1TCR = 0;   break;
	}
}

