#ifdef USE_FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#else
#include <micro214x.h>
#endif

#include "clock.h"

clock_time_t clock_time (void)
{
#ifdef USE_FREERTOS
  return xTaskGetTickCount ();
#else
  return t1_get_ms();
#endif
}
