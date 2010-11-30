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

int flashen_button(void);

int flashen_button(void)  /* returns true when button is on */
{
	return IO_input(P1_16);//!(IOPIN1&(1<<16));  // setups pin as input automatically
}


int main(void)
{

	LEDS_init();	/* init the LEDs for this board  */
	IO_init(FAST_IO);
	
	PWM_init(PWM_1|PWM_4); /* P0.0, P0.8 */
	PWM_frequency(50); /* 50 Hz PWM output, 20ms */
	

	LEDS_on(LED3);
	
	while(1) 
	{
	
		if (flashen_button())
		{
			PWM_pulsewidth_us (PWM_1, 1500); // Center
			PWM_pulsewidth_us (PWM_4, 1500); // Center
			LEDS_on(LED2);
		}
		else
		{
			PWM_pulsewidth_us (PWM_1, 1000); // 45º left
			PWM_pulsewidth_us (PWM_4, 2000); // 45º right
			LEDS_off(LED2);
		}	
	}
}




