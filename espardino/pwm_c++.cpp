#include <micro214x.h>

#include <pwm_c++.h>
#include <pwm.h>

/*class PWM 
{
  private:
	int m_pin;
	int m_freq;
	int m_duty;
 */


PWM::PWM()
{
	m_pin=-1;
	m_freq=50;
	m_duty=10;
}

int PWM::attach(int pin)
{
	switch(pin)
	{
		case P0_0: m_pin=PWM_1; break;
		case P0_7: m_pin=PWM_2; break;
		case P0_1: m_pin=PWM_3; break;
		case P0_8: m_pin=PWM_4; break;
		case P0_21: m_pin=PWM_5; break;
		case P0_9: m_pin=PWM_6; break;
	}	

	if (m_pin!=-1)
	{
		PWM_init(m_pin);
		return 1;
	} 
	else
	{
		return -1;
	}
	
}


int PWM::setFrequency(int freq)
{
	if (m_pin!=-1) PWM_frequency(freq);
	return freq;
}

int PWM::setDutyPercent(int duty_percent)
{
	if (m_pin!=-1) PWM_dutycycle(m_pin,duty_percent);
	return duty_percent;	
		

}

int PWM::setPusewidthUS(int duty_us)
{
	if (m_pin!=-1) PWM_pulsewidth_us(m_pin,duty_us);
	return duty_us;
}

int PWM::setPulsewidthCount(int duty_cycles)
{
	if (m_pin!=-1) PWM_length(m_pin,duty_cycles);
	return duty_cycles;
}

int PWM::getCycleCount()
{
		return -1;
}



