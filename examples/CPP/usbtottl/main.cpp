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
	Serial usb(USB_TX,USB_RX);
	Serial uart0(P0_0, P0_1); /* U0_TX , U0_RX */
    
	uart0.setBps(38400); /* set 38400 bps on the UART */
	
	while(1) {
		/* if something received on usb resend to uart0 */
		if (usb.kbhit())   uart0.send(usb.recv());
		
		/* if something received on uart0 resend to usb */
		if (uart0.kbhit()) usb.send(uart0.recv());
	}

	return 0;
}