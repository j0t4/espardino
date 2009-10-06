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
#include <string.h>
#include <micro214x.h>
#include <stdio.h>
#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void vATaskFunction( void *pvParameters );
void vBTaskFunction( void *pvParameters );
void vCTaskFunction( void *pvParameters );



int main(void)
{

	xTaskHandle xHandleA,xHandleB,xHandleC;

        IO_init(FAST_IO);

	LEDS_init();	/* init the LEDs for this board  */


    
	xTaskCreate( vATaskFunction, (const signed char*)"TASKLEDS_A", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xHandleA );
	xTaskCreate( vBTaskFunction, (const signed char*)"TASKLEDS_B", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xHandleB );
	xTaskCreate( vCTaskFunction, (const signed char*)"TASKLEDS_C", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xHandleC );
	
	vTaskStartScheduler(); /* start the task scheduler, all task start now */
	
}


void vATaskFunction( void *pvParameters )
{

	portTickType lastTickTime;
 
	lastTickTime = xTaskGetTickCount ();


    for( ;; )
    {
		LEDS_on(LED1);
		vTaskDelayUntil (&lastTickTime, 100);	/* wait until lastTickTime + 100ms */
		LEDS_off(LED1);
		
		vTaskDelayUntil (&lastTickTime, 400);  /* wait until lastTickTime + 400ms */
		
		
    }
 }


void vBTaskFunction( void *pvParameters )
{

	portTickType lastTickTime;
 
	lastTickTime = xTaskGetTickCount ();


    for( ;; )
    {
		LEDS_on(LED2);
		vTaskDelayUntil (&lastTickTime, 50);
		LEDS_off(LED2);
		
		
		vTaskDelayUntil (&lastTickTime, 400);
		
		
    }
 }
 
 void vCTaskFunction( void *pvParameters )
{

	portTickType lastTickTime;
 
	lastTickTime = xTaskGetTickCount ();


    for( ;; )
    {
		LEDS_on(LED3);
		vTaskDelayUntil (&lastTickTime, 25);
		LEDS_off(LED3);
		
		vTaskDelayUntil (&lastTickTime, 400);
    }
 }


