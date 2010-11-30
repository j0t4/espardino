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

#include <LPC214x.h>
#include <vcom.h>
#include <string.h>
#include <micro214x.h>
#include <stdio.h>
#include <stdlib.h>
#include "servo.hpp"



void limit_angle(double *angle)
{
	if (*angle>180.0f) *angle=180.0f;
	if (*angle<000.0f) *angle=000.0f;
}

int main(void)
{
	double angle;
	char key;
	Serial usb(USB_TX,USB_RX);
	MyServo my_servo;
	
	Led led3(LED3);
	Led led1(LED1);
	
	my_servo.attach(P0_0);
	
	key =  usb.recv();
	
	usb.puts("Servo move=> + ->right / - ->left /  s->stop\n");
	
	angle = 0;
	
	while (1)
	{
		key = usb.recv();

		if (key=='+')
		{
			angle = angle + 0.5f;
			limit_angle(&angle);
			my_servo.move(angle);
			usb.printf("+ current angle: %3.2g, that's %d us\n",angle,my_servo.get_us());
			led1.off();
			led3.on();
		}
		else if (key=='-')
		{
			angle = angle -0.5f;
			limit_angle(&angle);
			my_servo.move(angle);
			usb.printf("+ current angle: %3.2g, that's %d us\n",angle,my_servo.get_us());
			led1.on(); 
			led3.off();
		}
		else if (key=='s')
		{
			led1.off(); led3.off();
			my_servo.stop();
			usb.printf("s stopped\n");
		}
	}
	
}




