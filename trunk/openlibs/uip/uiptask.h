#ifndef UIP_TASK_H
#define UIP_TASK_H

#include "uip/uip.h"

//
//
//
typedef enum
{
  UIPSTATE_OK = 0,
  UIPSTATE_ALREADYRUNNING,
  UIPSTATE_NOTRUNNING,
  UIPSTATE_TASKDIDNTSTART,
  UIPSTATE_ENCJ28C60ERR,
}
uipState_e;

//
//
//
void uipFastPoll (struct uip_conn *conn);
portBASE_TYPE uipIsRunning (void);
uipState_e uipStart (int doDHCP);
uipState_e uipStop (void);

#endif
