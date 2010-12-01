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

int main(void)
{
	int i;
	Serial usb(USB_TX,USB_RX);
	
	
	DigitalIO p1(P0_28);
	DigitalIO p2(P0_29);
	
	DigitalBus bus1(P1_17,P1_18,P1_19,P1_20,P1_21,P1_22,P1_23,P1_17); /*8bit bus*/
	
	i =0;
	while(1) 
	{
			/* reads the input from P1, writes it on P2 */
			if (p1.read()) p2.on();
			else		   p2.off();
			bus1.write(i);
			i=i+1;
			wait(0.5); /* delay for 0.5 seconds */
	}	

	return 0;
}