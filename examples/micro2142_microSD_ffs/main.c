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
#include <ff.h>



WORD acc_files, acc_dirs;
FILINFO Finfo;

int main(void)
{

	char key,dat;
	int res;
	unsigned int read_count;
	
	FATFS fs;				/* Pointer to file system object */
	FIL fsrc;
	
	
	LEDS_init();	/* init the LEDs for this board  */
	VCOM_init();	/* init the USB Virtual COM port */
	
	xprintf_output(&VCOM_putchar);	/* define the output for the xprintf/xputs */

	
	LEDS_on(LED3);
	
	
	
	
	while(1) {
	 
		xprintf("Push any key to open hello.txt\r\n");
		key = VCOM_getchar();
		f_mount(0,&fs); // mount drive 0 on filesystem
		
		res = f_open(&fsrc, "hello.txt", FA_OPEN_EXISTING | FA_READ);
		
		if (res!=FR_OK)
		{
			xprintf("unable to open hello.txt  res= %d\r\n",res);
		}
		else
		
		{
		
			while((f_read(&fsrc, &dat, 1, &read_count)==FR_OK)&&read_count!=0)
			{
					VCOM_putchar(dat);
			}
			
			f_close(&fsrc);
		}	
		
		f_mount(0,NULL); // unmount drive 0
		
	}
	
}




