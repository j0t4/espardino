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

#include <micro214x.h>
#include <vcom.h>
extern "C"
{
	#include <freertos/FreeRTOS.h>
	#include <freertos/task.h>
	int VCOM_RT_init();
	int VCOM_init()
	{
		return VCOM_RT_init();
	}
}
void vATaskFunction( void *pvParameters );
void vBTaskFunction( void *pvParameters );
void vCTaskFunction( void *pvParameters );


int main(void)
{

	xTaskHandle xHandleA,xHandleB,xHandleC;

	Serial usb(USB_TX,USB_RX);	
	
	xTaskCreate( vATaskFunction, (const signed char*)"TASKLEDS_A", configMINIMAL_STACK_SIZE, &usb , tskIDLE_PRIORITY+1, &xHandleA );
	xTaskCreate( vBTaskFunction, (const signed char*)"TASKLEDS_B", configMINIMAL_STACK_SIZE, &usb , tskIDLE_PRIORITY+1, &xHandleB );
	xTaskCreate( vCTaskFunction, (const signed char*)"TASKLEDS_C", configMINIMAL_STACK_SIZE, &usb , tskIDLE_PRIORITY+1, &xHandleC );

	vTaskStartScheduler(); /* start the task scheduler, all task start now */

}


void vATaskFunction( void *pvParameters )
{
	Serial *usb=(Serial*)pvParameters;
	Led led1(LED1);

	portTickType lastTickTime;
	lastTickTime = xTaskGetTickCount ();


    for( ;; )
    {
    	led1.on();
		vTaskDelayUntil (&lastTickTime, 100);	/* wait until lastTickTime + 100ms */
		led1.off();

		xprintf("** [A] TICK: %d\r\n",(int)lastTickTime);

		vTaskDelayUntil (&lastTickTime, 400);  /* wait until lastTickTime + 400ms */


    }
 }


void vBTaskFunction( void *pvParameters )
{
	Serial *usb=(Serial*)pvParameters;
	Led led2(LED2);

	portTickType lastTickTime;
	lastTickTime = xTaskGetTickCount ();


    for( ;; )
    {
		led2.on();
		vTaskDelayUntil (&lastTickTime, 50);
		led2.off();

		usb->printf("** [B] TICK: %d\r\n",(int)lastTickTime);

		vTaskDelayUntil (&lastTickTime, 400);


    }
 }

 void vCTaskFunction( void *pvParameters )
{
	Serial *usb=(Serial*)pvParameters;
	Led led3(LED3);

	portTickType lastTickTime;
	lastTickTime = xTaskGetTickCount ();


    for( ;; )
    {
		led3.on();
		vTaskDelayUntil (&lastTickTime, 25);
		led3.off();

		vTaskDelayUntil (&lastTickTime, 400);
    }
 }


