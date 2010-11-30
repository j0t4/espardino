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

void print_banner(void);

void print_banner(void)
{
	xprintf("\n\r");
	xprintf("http://www.espardino.com micro214x ADC test        \n\r");
	xprintf("--------------------------------------------------------\n\r");
	xprintf("\n\r");
	xprintf(" push 'a' to read AD0.1 on P0.28\n\r");
	xprintf("      'b' to read AD0.2 on P0.29\n\r");
	xprintf("\n\r");
	xprintf("      please, remember to set VREF pin to 3.3V or desired\n\r");
	xprintf("      analog reference voltage within specifications\n\r");
	xprintf("\n\r");
}

int main(void)
{

	char key;
	const char *port;
	int	ADC_value;
	float V3_3;
	
	LEDS_init();	/* init the LEDs for this board  */
	VCOM_init();	/* init the USB Virtual COM port */
	
	ADC_init(AD0_1|AD0_2); /* enable pins as analog input */
	
	xprintf_output(&VCOM_putchar);	/* define the output for the xprintf/puts */

	
	LEDS_on(LED3);
	
	
	while(1) 
	{
	
			key = VCOM_getchar();
			
			switch(key)
			{
				case 'a': port="AD0.1";   
						  ADC_value = ADC_read(AD0_1);
						break;
						
				case 'b': port="AD0.2";
						  ADC_value = ADC_read(AD0_2);
						break;
				
				default:
					print_banner();
					continue; // skip back to while(1)
			}
			
			// value with reference to a 3.3V VREF signal
			
			V3_3 = ((float)ADC_value)*3.3/1023.0;
			
			
			xprintf("%s 10bit value (0..1023) is %4d, or %1.3gV\n\r",port,ADC_value,V3_3);
			
	}
	
	
	
}




