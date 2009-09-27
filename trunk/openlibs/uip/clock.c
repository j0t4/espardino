#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "clock.h"

clock_time_t clock_time (void)
{
  return xTaskGetTickCount ();
}
