
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <micro214x.h>


extern xSemaphoreHandle xENC28J60Semaphore;
void enc28j60_eint0ISR_Handler (void);

void enc28j60_eint0ISR_Handler (void)
{

  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  EXTINT |= 1; // CLEAR EINT0

  
  xSemaphoreGiveFromISR (xENC28J60Semaphore, &xHigherPriorityTaskWoken);

  VICVectAddr = 0; // ACK VIC interrupt

  if (xHigherPriorityTaskWoken)
    portYIELD_FROM_ISR ();
}

void enc28j60_eint0ISR (void) __attribute__ ((naked));
void enc28j60_eint0ISR (void)
{
  portSAVE_CONTEXT ();
  enc28j60_eint0ISR_Handler();
  portRESTORE_CONTEXT ();
}