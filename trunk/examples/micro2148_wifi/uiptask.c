
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

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <micro214x.h>
#include <rtc.h>

#undef HTONS
#include "hardware/enc28j60.h"
#include "uip/uip.h"
#include "uip/uip_arp.h"
#include "apps/webserver/httpd.h"
#include "apps/telnetd/telnetd.h"
#include "apps/dhcpc/dhcpc.h"
#include "apps/sntp/sntp.h"
#include "uiptask.h"
#include <vcom.h>

#define USE_ZG2100 1

#ifdef USE_ZG2100
#include "ZG2100MAC.h"
#endif

//
//  The start of the uIP buffer, which will contain the frame headers
//
#define pucUIP_Buffer ((struct uip_eth_hdr *) &uip_buf [0])

//
//  uIP update frequencies
//
#define RT_CLOCK_SECOND   (configTICK_RATE_HZ)
#define uipARP_FREQUENCY  (1000/8)
#define uipMAX_BLOCK_TIME (4)

xTaskHandle xUIPTaskHandle;

//
//
//
u8_t uip_buf [UIP_BUFSIZE + 8] __attribute__ ((aligned (4)));

static int useDHCP = TRUE;
static xQueueHandle xFastPollQueue = NULL; 

//
//
//
static portTASK_FUNCTION (vUIPTask, pvParameters)
{
  portBASE_TYPE xARPTimer;
  volatile portBASE_TYPE *errFlag = (portBASE_TYPE *) pvParameters;
  static volatile portTickType xStartTime, xCurrentTime;
  int last_connected = 0;
  int sw;
  signed portBASE_TYPE hadData;
  unsigned char wepkey[13];
  int alreadySetup=0;

   
  xprintf("GO!\r\n");

  
  if (!xFastPollQueue)
    xFastPollQueue = xQueueCreate (1, (unsigned portBASE_TYPE) sizeof (struct uip_conn *));

  //
  //  Initialize the uIP TCP/IP stack
  //
  uip_init ();
  uip_arp_init ();

  
  //
  //  Initialize the Ethernet controller hardware
  //

#ifdef USE_ENC28J60    
  if (!enc28j60Init ())
  {
    if (errFlag)
      *errFlag = 1;
	
    while (1)
      vTaskDelay (1000 / portTICK_RATE_MS);
  }
  xprintf("Initialization OK!! :)\n");

  if (errFlag)
    *errFlag = 0;

  //
  //  Either we've got an address, or we need to request one
  //
  if (useDHCP)
  {
    dhcpc_init (uip_ethaddr.addr, sizeof (uip_ethaddr.addr));
    dhcpc_request ();
  }
  else
  {
  
	no_dhcp_config();
  }

#endif

// ARP 

#ifdef USE_ZG2100

	delay_ms(100);
	//VCOM_getchar();
  //      ZG2100_SetOutput(&VCOM_putchar_nonblock);
	ZG2100_Init();
	ZG2100_LinkMgrInit();
	
	
	//ZG2100_LinkMgrSetNextMode(kZGLMNetworkModeAdhoc);
	//ZG2100_SetSSID("EspardinoWifi", strlen("EspardinoWifi"));
	if (useDHCP)
	{
		ZG2100_LinkMgrSetNextMode(kZGLMNetworkModeInfrastructure);
		ZG2100_SetAllRfChannels();
		ZG2100_SetSSID("ajocasa",strlen("ajocasa"));
		ZG2100_SetWEPKeyLong("pelayopelayop",0);
		ZG2100_SetEncryptionType(kKeyTypeWep);
		ZG2100_SetAuthType(kZGAuthAlgOpen);
	}
	else
	{
		ZG2100_LinkMgrSetNextMode(kZGLMNetworkModeAdhoc);
		ZG2100_SetAllRfChannels();
		ZG2100_SetSSID("EspardinoWifi",strlen("EspardinoWifi"));
		ZG2100_SetEncryptionType(kKeyTypeNone);
		ZG2100_SetAuthType(kZGAuthAlgOpen);
		//ZG2100_SetTxRate(kZGRFRateOneMbps);
	}
	
	
	//ZG2100_SetWEPKey("\x91\x36\x41\x46\x00", 0, kZGWEPKeyLenShort);

#endif

  //
  //  Initialise the local timers
  //
  xStartTime = xTaskGetTickCount ();
  xARPTimer = 0;

  //
  //  The big loop
  //

  sw=0;

  while (1)
  {
    /* enc28j60WaitForData() returns pdTRUE if we were woken by an 
       interrupt, otherwise pdFALSE if the timer expired.  If pdTRUE,
       then the ENC28J60 has data ready for us. */
#ifdef USE_ENC28J60
    if (enc28j60WaitForData (uipMAX_BLOCK_TIME) == pdTRUE)
    {
		if ((uip_len = enc28j60Receive ()) > 0)
		{
#endif

#ifdef USE_ZG2100
	
	
	if ((ZG2100_IsLinked()!=last_connected))
	{
		last_connected=ZG2100_IsLinked();
		
		if (last_connected)
		{
		  memcpy(uip_ethaddr.addr,ZG2100_GetMacAddr(),6);
		  if (useDHCP)
		  {
			
			dhcpc_init (uip_ethaddr.addr, sizeof (uip_ethaddr.addr));
			dhcpc_request ();
		  }
		  else
		  {
		  
			if (!alreadySetup)
			{
				no_dhcp_config();
				alreadySetup=1;
			}
			
		  }
				
		}
		else
		{
			// we got disconnected, ask the linkmanager to connect again
			ZG2100_LinkMgrInit();
			ZG2100_LinkMgrSetNextMode(kZGLMNetworkModeInfrastructure);
			
		}
			
	}
	
	
	
	
	hadData = ZG2100_WaitData (uipMAX_BLOCK_TIME);
    
		
	ZG2100_Process();
	ZG2100_LinkMgr();
	
	
	
	
	//if (hadData==pdTRUE) VCOM_putchar('*'); else VCOM_putchar('.');
	
		
	if ((uip_len = ZG2100_GetUip((unsigned char *)uip_buf,UIP_BUFSIZE)) > 0)	
	{
		//if (hadData==pdTRUE) VCOM_putchar_nonblock('.');
	
		if (uip_len>0)
        {
#endif
      /* Let the network device driver read an entire IP packet
         into the uip_buf. If it returns > 0, there is a packet in the
         uip_buf buffer. */
      
        /* A packet is present in the uIP buffer. We call the
           appropriate ARP functions depending on what kind of packet we
           have received. If the packet is an IP packet, we should call
           uip_input() as well. */
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
			
#ifdef USE_ENC28J60			
            enc28j60Send ();
#endif
			
#ifdef USE_ZG2100
			ZG2100_PutUip((unsigned char *)uip_buf,uip_len);
#endif
          }
        }
        else if (pucUIP_Buffer->type == htons (UIP_ETHTYPE_ARP))
        {
          uip_arp_arpin ();

          /* If the above function invocation resulted in data that
             should be sent out on the network, the global variable
             uip_len is set to a value > 0. */  
          if (uip_len > 0)
		  {
            #ifdef USE_ENC28J60			
            enc28j60Send ();
			#endif
			
			#ifdef USE_ZG2100
			ZG2100_PutUip((unsigned char *)uip_buf,uip_len);
			#endif
		  }
        }
      }
    }
    else
    {
      struct uip_conn *conn;


      //
      //  If there's data in the short circuit queue, it means that
      //  uipFastPoll() was called with a connection number, and
      //  we need to poll that connection.
      //
      if (xFastPollQueue && (xQueueReceive (xFastPollQueue, &conn, 0) == pdTRUE))
      {
        uip_poll_conn (conn);

        if (uip_len > 0)
        {
          uip_arp_out ();
			#ifdef USE_ENC28J60			
            enc28j60Send ();
			#endif
			
			#ifdef USE_ZG2100
			ZG2100_PutUip((unsigned char *)uip_buf,uip_len);
			#endif
        }
      }

      /* The poll function returned 0, so no packet was
         received. Instead we check if it is time that we do the
         periodic processing. */
      xCurrentTime = xTaskGetTickCount ();

      if ((xCurrentTime - xStartTime) >= RT_CLOCK_SECOND)
      {
        portBASE_TYPE i;

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
            #ifdef USE_ENC28J60			
            enc28j60Send ();
			#endif
			
			#ifdef USE_ZG2100
			ZG2100_PutUip((unsigned char *)uip_buf,uip_len);
			#endif
          }
        }

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
            #ifdef USE_ENC28J60			
            enc28j60Send ();
			#endif
			
			#ifdef USE_ZG2100
			ZG2100_PutUip((unsigned char *)uip_buf,uip_len);
			#endif
          }
        }
#endif /* UIP_UDP */

        /* Periodically call the ARP timer function. */
        if (++xARPTimer == uipARP_FREQUENCY)
        { 
          uip_arp_timer ();
          xARPTimer = 0;
        }
      }
    }
  }
}

//
//
//
void uipFastPoll (struct uip_conn *conn)
{
  if (xFastPollQueue)
    xQueueSend (xFastPollQueue, &conn, portMAX_DELAY);
}

//
//
//
portBASE_TYPE uipIsRunning (void)
{
  return xUIPTaskHandle ? 1 : 0;
}

portBASE_TYPE uiptask_errFlag = -1;
uipState_e uipStart (int doDHCP)
{
  volatile portTickType xTicks;

   xTaskHandle xTASKUip;
  if (uipIsRunning ())
    return UIPSTATE_ALREADYRUNNING;
  
  useDHCP = doDHCP;

  

  xTASKUip = xTaskCreate (vUIPTask, (const signed portCHAR * const) "uIP2", 1400, &uiptask_errFlag, (tskIDLE_PRIORITY + 1), &xUIPTaskHandle);


  if (!xTASKUip) xprintf("ERROR NO uIP task!\r\n");

/*
  xTicks = xTaskGetTickCount ();

  while ((volatile portBASE_TYPE) errFlag == -1)
  {
    vTaskDelay (100 / portTICK_RATE_MS);

    if ((xTaskGetTickCount () - xTicks) > (5000 / portTICK_RATE_MS))
    {
      uipStop ();
      return UIPSTATE_TASKDIDNTSTART;
    }
  }

  if (errFlag == 1)
  {
    uipStop ();
    return UIPSTATE_ENCJ28C60ERR;
  }
*/
  return UIPSTATE_OK;
  
}

uipState_e uipStop (void)
{
  if (!uipIsRunning ())
    return UIPSTATE_NOTRUNNING;

  vTaskDelete (xUIPTaskHandle);
  xUIPTaskHandle = NULL;
 
			#ifdef USE_ENC28J60			
             enc28j60Deinit ();
			#endif
			
			#ifdef USE_ZG2100
			
			#endif  
  
  
  return UIPSTATE_OK;
}

extern struct uip_conn *stream_connection;

void dispatch_tcp_appcall (void)
{
  if (uip_conn->lport == HTONS (80))
    httpd_appcall ();

//  if (uip_conn->lport == HTONS (23))
//    telnetd_appcall ();

	if (uip_conn==stream_connection)
		streamer_appcall();
}

void dispatch_udp_appcall (void)
{
//  if (uip_udp_conn->rport == HTONS (123))
//    sntp_appcall ();
//  else

  if (uip_udp_conn->rport == HTONS (DHCPC_SERVER_PORT))
    dhcpc_appcall ();


   resolv_appcall();

}



#ifdef CFG_AUTO_SNTP
//
//
//
static void uipAutoSNTPTimeSynced (time_t *epochSeconds)
{
  if (*epochSeconds)
  {
    n32_t to;
      
    uip_gettimeoffset (&to);
    *epochSeconds += to;
    rtcSetEpochSeconds (*epochSeconds);
  }
}

static void uipAutoSNTP (void)
{
  uip_ipaddr_t addr;

  uip_getsntpaddr (&addr);

  if (!uip_iszeroaddr (&addr))
    sntpSync (&addr, uipAutoSNTPTimeSynced);
}
#endif


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

void dhcpc_configured (const dhcpcState_t *s)
{
  if (!s->ipaddr [0] && !s->ipaddr [1])
  {
    xprintf ("Can't get address via DHCP and no static address configured, stopping uIP task\n");
    uipStop ();
  }
  else 
    {
    xprintf ("\nIP address via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->ipaddr), uip_ipaddr2 (s->ipaddr), uip_ipaddr3 (s->ipaddr), uip_ipaddr4 (s->ipaddr));
	xprintf ("Netmask via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->netmask), uip_ipaddr2 (s->netmask), uip_ipaddr3 (s->netmask), uip_ipaddr4 (s->netmask));
	xprintf ("Router  via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->default_router), uip_ipaddr2 (s->default_router), uip_ipaddr3 (s->default_router), uip_ipaddr4 (s->default_router));
	xprintf ("DNS     via DHCP is %d.%d.%d.%d\n", uip_ipaddr1 (s->dnsaddr), uip_ipaddr2 (s->dnsaddr), uip_ipaddr3 (s->dnsaddr), uip_ipaddr4 (s->dnsaddr));
	
    uip_sethostaddr (s->ipaddr);
    uip_setnetmask (s->netmask);
    uip_setdraddr (s->default_router);
    uip_setsntpaddr (s->sntpaddr);
    uip_settimeoffset (&s->timeoffset);
	
	
	httpd_init ();
	resolv_init();
	resolv_conf(s->dnsaddr);
	streamer_init();

  }
}
