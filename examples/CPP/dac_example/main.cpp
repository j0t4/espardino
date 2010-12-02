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
// all copies or substantial portions of the Software(.
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
#include <dac.hpp>
#include <math.h>

/* builds a sin table from -0x7fff to 0x7fff (instead of -1 to 1) */

#define PI 3.14159265f
short int sinTable[360];
void buildSinTable()
{
	double sinDouble;
	for (int i=0;i<360;i++)
		{
			sinDouble = sin(PI/180.0*(double)i)*(double)0x7fff;
			sinTable[i]= (int)sinDouble;
		}
}

int main(void)
{
	int i,j;

	Serial usb(USB_TX,USB_RX);
	DigitalIO button(P1_16);
	Led led1(LED1);
	Led led2(LED2);

	DACBuffer dac;


	dac.setFrequency(44000); /* dac will output data @ 44khz */
							/* remember to connect VREF to 3.3V */
	buildSinTable();

	i=0; j=0;

	while(1)
	{
		i+=4;
		if (i>360) i=0;
		if (button.read()==0)
		{
			dac.add(sinTable[i]/2); /* button pushed, half amplitude */
			led2.on();
		}
		else
		{
			dac.add(sinTable[i]/4); /* quarter amplitude by default */
			led2.off();
		}
		if (++j%4096==0) led1 = !led1;
	}


}
