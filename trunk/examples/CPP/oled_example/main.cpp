////
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

int main(void)
{
	float x,y,dx,dy;
	Serial usb(USB_TX,USB_RX);
	SSD1308 oled;

	oled.attach(P0_19, /*MOSI*/
				P0_18, /*MISO*/
				P0_17, /*SCK*/
				P0_20, /*CS */
				P0_16, /*DC*/
				P0_15  /*RESET*/);

	oled.init();
	oled.setContrast(0x50);


	x=0;y=0;
	dx=0.7;dy=0.2;

	oled.setFont(FONT_6x8,FONT_NORMAL);

	while(1)
	{
		x+=dx;
		y+=dy;
		if (x>=127 || x<=0) dx=-dx;
		if (y>=63 || y<=0) dy=-dy;

		oled.clear();
		oled.plot(x,y,1);
		oled.gotoxy(x,y);
		oled.printf("(%d,%d) hello",(int)x,(int)y);
		oled.line(64,32,x,y,1);

		oled.dump();

	}
}
