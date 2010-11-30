//
// Copyright (c) 2009  http://www.espardino.com 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include <micro214x.h>
#include "servo.hpp"


MyServo::MyServo()
{
   min_us = 545;
   max_us = 2400;
   current_us = 0; // no PWM output by default (open servo)
   pwm_pin = 0;
}

int MyServo::attach(int pin)
{
	switch(pin)
	{
		case P0_0:	this->pwm_pin = PWM_1; break;
		case P0_7:	this->pwm_pin = PWM_2; break;
		case P0_1:	this->pwm_pin = PWM_3; break;
		case P0_8:	this->pwm_pin = PWM_4; break;
		case P0_21:	this->pwm_pin = PWM_5; break;
		case P0_9:	this->pwm_pin = PWM_6; break;
		default:
			this->pwm_pin = 0;
	}
	
	if (this->pwm_pin)
	{
		PWM_frequency(50);  // 50Hz -> 20ms cycle
		PWM_init(this->pwm_pin);
		this->stop();
	}
	
	return (this->pwm_pin!=0);
}

void MyServo::stop()
{
	if (this->pwm_pin)
	{
		PWM_pulsewidth_us(this->pwm_pin,0);
	}
}

void MyServo::move(double angle)
{
	int us_calc;
	
	this->current_angle = angle;
	
	if (this->pwm_pin)
	{
		
		us_calc = min_us + ((double)(max_us-min_us) * (angle/180.0f));
		
		if (us_calc<min_us) us_calc = min_us;
		if (us_calc>max_us) us_calc = max_us;
		
		current_us = us_calc;
		
		PWM_pulsewidth_us(this->pwm_pin,us_calc);
	}
}

