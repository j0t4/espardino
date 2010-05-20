#ifndef __TIMER_CPP_H
#define __TIMER_CPP_H

#define TIMER0 0
#define TIMER1 1

class Ticker
{
	private:
	int m_timer;
		
	public:
	static ptVoidFunction m_functionT0;
	static ptVoidFunction m_functionT1;
	
	Ticker(int timer=TIMER0);
	~Ticker();
	void attach(ptVoidFunction function);
	void attach(ptVoidFunction function,float period);
	void setFrequency(int freq);
	void setPeriod(float period);
	void start();
	void stop();
};

#endif