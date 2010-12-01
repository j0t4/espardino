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

#include <pwm_c++.hpp>
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



