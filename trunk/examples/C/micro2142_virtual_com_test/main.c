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

void print_banner(void)
{
	xprintf("\n");
	xprintf("http://www.espardino.com micro214x Virtual COM test\n");
	xprintf("--------------------------------------------------------\n");
	xprintf("\n");
	xprintf(" push 'a' to turn on LED1, 'A' to turn off LED1\n");
	xprintf("      'b' to turn on LED2, 'B' to turn off LED2\n");
	xprintf("      'c' to turn on LED3, 'C' to turn off LED3\n");
	xprintf("      'd' to turn on all,  'D' to turn off all\n");
	xprintf("\n");
}

int main(void)
{

	char key;
	
	LEDS_init();	/* init the LEDs for this board  */
	VCOM_init();	/* init the USB Virtual COM port */
	
	xprintf_output(&VCOM_putchar_nl);	/* define the output for the xprintf/xputs */

	
    LEDS_on(LED3); /* switch on LED3 */
	
	
	while(1) {
	
			key = VCOM_getchar();
			
			xprintf("You pushed the key %c\n",key);
			
			switch(key)
			{
				case 'a': LEDS_on(LED1);  break;
				case 'A': LEDS_off(LED1); break;
				case 'b': LEDS_on(LED2);  break;
				case 'B': LEDS_off(LED2); break;
				case 'c': LEDS_on(LED3);  break;
				case 'C': LEDS_off(LED3); break;
				case 'd': LEDS_on(LED1|LED2|LED3);  break;
				case 'D': LEDS_off(LED1|LED2|LED3); break;
				
				default:
					print_banner();
			}
			
	}
	
	
	
}




