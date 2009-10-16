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
#include <ZG2100MAC.h>
#include <netif.h>

char uip_buf[2048];

void dump_hex(char *buf, int len);

int main(void)
{
	netif *net;
	int useADHOC=0;
	int uip_len;

	IO_init(FAST_IO);

	LEDS_init();	/* init the LEDs for this board  */
    VCOM_init(); /* init the USB Virtual COM port */
	                /* We link with vcom_rtos lib, that's better designed
						for use with freertos */

    xprintf_output(&VCOM_putchar_nl);      /* define the output for the printf/puts */

	LEDS_off(LED1|LED2|LED3); /* switch off all the leds */

	delay_init();

	/* ZG2100 specific setup */

	ZG2100_SetOutput(&VCOM_putchar_nonblock);  // setup debug output (if you want to watch)
	ZG2100_Init();	// Init the driver
	ZG2100_LinkMgrInit();	// Init the link manager

	if (!useADHOC)
	{
		ZG2100_LinkMgrSetNextMode(kZGLMNetworkModeInfrastructure);
		ZG2100_SetAllRfChannels();
		ZG2100_SetSSID((unsigned char*)"ajocasa",7);
		ZG2100_SetWEPKeyLong((unsigned char*)"pelayopelayop",0);
		ZG2100_SetEncryptionType(kKeyTypeWep);
		ZG2100_SetAuthType(kZGAuthAlgOpen);
	}
	else
	{
		ZG2100_LinkMgrSetNextMode(kZGLMNetworkModeAdhoc);
		ZG2100_SetAllRfChannels();
		ZG2100_SetSSID((unsigned char*)"EspardinoWifi",13);
		ZG2100_SetEncryptionType(kKeyTypeNone);
		ZG2100_SetAuthType(kZGAuthAlgOpen);
		//ZG2100_SetTxRate(kZGRFRateOneMbps);
	}

	net = ZG2100_GetNetIf();
	while(1)
	{

		net->process();

		if (net->is_linked()) LEDS_on(LED1);
		else		  	      LEDS_off(LED1);

		uip_len = net->rx_data((unsigned char *)uip_buf,2000);

		if (uip_len>0)
		{
			xprintf("[%d]\r\n",t1_get_ms());
			LEDS_on(LED2);
			dump_hex(uip_buf,uip_len);
			LEDS_off(LED2);
		}


	}

}

void dump_hex(char *buf, int len)
{
	int l,i;

	xprintf("%4d> ",len);

	for (i=0,l=0;i<len;i++)
	{
		xprintf("%02x ",buf[i]);

		if (++l>=16)
		{
			if (i<(len-1))	xprintf("\r\n      ");
			l=0;
		}
	}

	xprintf("\r\n");

}






