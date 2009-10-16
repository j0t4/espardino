UIP=$(ESPARDINO)/src/openlibs/uip

SRC_UIP = $(UIP)/clock.c \
		  $(UIP)/uip/psock.c \
		  $(UIP)/uip/uip.c \
		  $(UIP)/uip/uip-fw.c \
		  $(UIP)/uip/uip-neighbor.c \
		  $(UIP)/uip/uip_timer.c \
		  $(UIP)/uip/uiplib.c \
		  $(UIP)/uip/uip_arch.c \
		  $(UIP)/uip/uip_arp.c

SRC_UIP_RESOLV = $(UIP)/apps/resolv/resolv.c
SRC_UIP_DHCP = 	$(UIP)/apps/dhcpc/dhcpc.c

OPTS_UIP = -I$(UIP)
	  
		  
