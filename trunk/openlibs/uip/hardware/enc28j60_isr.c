#ifdef USE_FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

extern xSemaphoreHandle xENC28J60Semaphore;
#endif

#include <micro214x.h>
#include <LPC214x.h>


void enc28j60_eint0ISR_Handler (void);
static int enc28j60_had_interrupt=0;
void enc28j60_eint0ISR_Handler (void)
{

#ifdef USE_FREERTOS
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
#endif

  EXTINT |= 1; // CLEAR EINT0

#ifdef USE_FREERTOS
  xSemaphoreGiveFromISR (xENC28J60Semaphore, &xHigherPriorityTaskWoken);
#endif

  enc28j60_had_interrupt=1;
  VICVectAddr = 0; // ACK VIC interrupt

#ifdef USE_FREERTOS
  if (xHigherPriorityTaskWoken)
    portYIELD_FROM_ISR ();
#endif

}
#ifdef USE_FREERTOS
void enc28j60_eint0ISR (void) __attribute__ ((naked));
#else
void enc28j60_eint0ISR (void) __attribute__ ((interrupt("IRQ")));
#endif

void enc28j60_eint0ISR (void)
{

#ifdef USE_FREERTOS
	portSAVE_CONTEXT ();
#endif
  enc28j60_eint0ISR_Handler();
#ifdef USE_FREERTOS
  portRESTORE_CONTEXT ();
#endif
}
