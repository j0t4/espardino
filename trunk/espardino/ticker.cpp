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

