
/*
 * Modified from an original work that is Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 */

//
//  Standard includes
//
#include <stdlib.h>
#include <stdio.h>

#ifdef USE_FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#endif

#include <micro214x.h>
#include <rtc.h>

#undef HTONS

#include <netif.h>
#include "uip/uip.h"
#include "uip/uip_arp.h"
#include "apps/dhcpc/dhcpc.h"
#include "uiptask.h"

//
//  The start of the uIP buffer, which will contain the frame headers
//
#define pucUIP_Buffer ((struct uip_eth_hdr *) &uip_buf [0])

//
//  uIP update frequencies
//

#ifdef USE_FREERTOS
#define RT_CLOCK_SECOND   (configTICK_RATE_HZ)
xTaskHandle xUIPTaskHandle;
static xQueueHandle xFastPollQueue = NULL;
#else
#define RT_CLOCK_SECOND   (1000)
#define xTaskGetTickCount  t1_get_ms
#define vTaskDelay(x)	   delay_ms(x)
#endif

#define uipARP_FREQUENCY  (1000/8)
#define uipMAX_BLOCK_TIME (4)


u8_t uip_buf [UIP_BUFSIZE + 8] __attribute__ ((aligned (4)));

static int useDHCP = TRUE;
static netif *netIF;
static int uip_task_last_linked = 0;

unsigned int xCurrentTime;
unsigned int xStartTime;
unsigned int xARPTimer;

int uip_task_init(int use_dhcp, netif *pnetIF)
{
	netIF = pnetIF;
	useDHCP = use_dhcp;

#ifdef USE_FREERTOS
	  if (!xFastPollQueue)
	    xFastPollQueue = xQueueCreate (1, (unsigned portBASE_TYPE) sizeof (struct uip_conn *));
#endif

    uip_init ();
    uip_arp_init ();

    xStartTime =   xTaskGetTickCount();
    xARPTimer = 0;

}

int uip_task_process()
{
	int is_linked;

	is_linked = netIF->is_linked();

	if (is_linked!=uip_task_last_linked)
	{
		uip_task_last_linked=is_linked;

		if (is_linked)
		{
		  memcpy(uip_ethaddr.addr,netIF->get_mac_address(),6);
		  if (useDHCP)
		  {

			dhcpc_init (uip_ethaddr.addr, sizeof (uip_ethaddr.addr));
			dhcpc_request ();
		  }

		}
		else
		{
			// we got disconnected, ask the interface to link again
			netIF->relink();
		}

	}


	/* this will yield processor if we're using freertos enabled driver */

	netIF->wait_data(uipMAX_BLOCK_TIME);

	netIF->process();  /* do internal processes of the driver */

	uip_len = netIF->rx_data(uip_buf,UIP_BUFSIZE);

	/*****************************************************************************************/

	if (uip_len>0)
    {
        /* if the incomming packet is IP handle ARP, then IP */

        if (pucUIP_Buffer->type == htons (UIP_ETHTYPE_IP))
        {
          uip_arp_ipin ();
          uip_input ();

          /* If the above function invocation resulted in data that
             should be sent out on the network, the global variable
             uip_len is set to a value > 0. */
          if (uip_len > 0)
          {
            uip_arp_out ();
            netIF->tx_data((unsigned char *)uip_buf,uip_len);
          }
        } /* if the incoming packet is ARP type then let UIP handle it for ARP */
        else if (pucUIP_Buffer->type == htons (UIP_ETHTYPE_ARP))
        {
          uip_arp_arpin ();

          if (uip_len > 0)
			netIF->tx_data((unsigned char *)uip_buf,uip_len);

        }
    }
    else
    {
#ifdef USE_FREERTOS
      struct uip_conn *conn;
      if (xFastPollQueue && (xQueueReceive (xFastPollQueue, &conn, 0) == pdTRUE))
      {
        uip_poll_conn (conn);

        if (uip_len > 0)
        {
            uip_arp_out ();
			netIF->tx_data((unsigned char *)uip_buf,uip_len);
        }
      }
#endif

      /* The poll function returned 0, so no packet was
         received. Instead we check if it is time that we do the
         periodic processing. */
      xCurrentTime = xTaskGetTickCount ();

      if ((xCurrentTime - xStartTime) >= RT_CLOCK_SECOND)
      {

    	int i;

        /* Reset the timer. */
        xStartTime = xCurrentTime;

        /* Periodic check of all connections. */
        for (i = 0; i < UIP_CONNS; i++)
        {
          uip_periodic (i);

          /* If the above function invocation resulted in data that
             should be sent out on the network, the global variable
             uip_len is set to a value > 0. */
          if (uip_len > 0)
          {
              uip_arp_out ();
              netIF->tx_data((unsigned char *)uip_buf,uip_len);
          }
        }

		/* check for UDP connections , may be they have data to be sent too */
#if UIP_UDP
        for (i = 0; i < UIP_UDP_CONNS; i++)
        {
          uip_udp_periodic (i);

          /* If the above function invocation resulted in data that
             should be sent out on the network, the global variable
             uip_len is set to a value > 0. */
          if (uip_len > 0)
          {
              uip_arp_out ();
              netIF->tx_data((unsigned char *)uip_buf,uip_len);
          }
        }
#endif /* UIP_UDP */

        /* Periodically call the ARP timer function. */
        if (++xARPTimer == uipARP_FREQUENCY)
        {
          uip_arp_timer ();
          xARPTimer = 0;
        }
      } /*  if ((xCurrentTime - xStartTime) >= RT_CLOCK_SECOND) */
    } /* if data received {..} else { */
}

//
//
//
void uip_task_FastPoll (struct uip_conn *conn)
{
#ifdef USE_FREERTOS
  if (xFastPollQueue)
    xQueueSend (xFastPollQueue, &conn, portMAX_DELAY);
#endif
}


void dispatch_tcp_appcall_uip(void);
void dispatch_udp_appcall_uip(void);
void dhcpc_configured_uip (const dhcpcState_t *s);

void dispatch_tcp_appcall (void)
{
	dispatch_tcp_appcall_uip();
}

void dispatch_udp_appcall (void)
{

  if (uip_udp_conn->rport == HTONS (DHCPC_SERVER_PORT))
    dhcpc_appcall ();

   resolv_appcall();

   dispatch_udp_appcall_uip();

}

/*
void no_dhcp_config()
{
	uip_ipaddr_t addr;

 uip_ipaddr(&addr, 169,254,1,2);
 uip_sethostaddr(&addr);

 uip_ipaddr(&addr, 255,255,0,0);
 uip_setnetmask(&addr);

 uip_ipaddr(&addr, 169,254,1,1);
 uip_setdraddr(&addr);

  uip_ipaddr(&addr, 194,179,1,100);
 uip_setdraddr(&addr);

 httpd_init();
 resolv_init();
 streamer_init();
}
*/
void dhcpc_configured (const dhcpcState_t *s)
{
  if (!s->ipaddr [0] && !s->ipaddr [1])
  {
 //   xprintf ("Can't get address via DHCP and no static address configured, stopping uIP task\n");

  }
  else
    {
	  /*
    xprintf ("\nIP address via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->ipaddr), uip_ipaddr2 (s->ipaddr), uip_ipaddr3 (s->ipaddr), uip_ipaddr4 (s->ipaddr));
	xprintf ("Netmask via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->netmask), uip_ipaddr2 (s->netmask), uip_ipaddr3 (s->netmask), uip_ipaddr4 (s->netmask));
	xprintf ("Router  via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->default_router), uip_ipaddr2 (s->default_router), uip_ipaddr3 (s->default_router), uip_ipaddr4 (s->default_router));
	xprintf ("DNS     via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->dnsaddr), uip_ipaddr2 (s->dnsaddr), uip_ipaddr3 (s->dnsaddr), uip_ipaddr4 (s->dnsaddr));
*/
    uip_sethostaddr (s->ipaddr);
    uip_setnetmask (s->netmask);
    uip_setdraddr (s->default_router);
    uip_setsntpaddr (s->sntpaddr);
    uip_settimeoffset (&s->timeoffset);


	resolv_conf(s->dnsaddr);
	dhcp_configured_uip(s);

  }
}
