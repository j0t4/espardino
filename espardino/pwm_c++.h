#ifndef __PWM_CPP_H
#define __PWM_CPP_H

class PWM 
{
  private:
	int m_pin;
	int m_freq;
	int m_duty;
  public:
	PWM();
	int attach(int pin);
	int setFrequency(int freq);
	int setDutyPercent(int duty_percent);
	int setPusewidthUS(int duty_us);
	int setPulsewidthCount(int duty_cycles);
	int getCycleCount();
};

#endif
