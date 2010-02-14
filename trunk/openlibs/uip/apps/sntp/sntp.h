
#ifndef _SNTP_H_
#define _SNTP_H_

#include <time.h>

#include "../../uip/uip_timer.h"

//
//
//
#define SNTP_TIMEOUT 10
#define SNTP_RETRIES 10

//
//
//
typedef struct sntpState_s
{
	struct pt pt;
	struct uip_udp_conn *conn;
	struct timer timer;
	int retry;
  void (*onSyncCallback) (time_t *epochSeconds);
}
sntpState_t;

typedef sntpState_t uip_udp_appstate_sntp;

//
//
//
int sntpSync (uip_ipaddr_t *ipaddr, void (*syncedCallback) (time_t *epochSeconds));
void sntp_appcall (void);

#endif
