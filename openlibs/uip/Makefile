LIBNAME	= uip

ifeq ($(MODE),thumb)
	MODE_DIR = thumb
        MODE=thumb
else
	MODE_DIR = .
    MODE=arm
endif

ifeq ($(RTOS),freertos)
	RTOS=$(ESPARDINO)/src/FreeRTOS
	DEFINES=-DUSE_FREERTOS
else
	RTOS=
	DEFINES=
endif


ifeq ($(INTERWORK),no)
	INTERWORK_PARAM = 
	TARGET_LIBDIR = $(MODE_DIR)
else	
	INTERWORK_PARAM = -mthumb-interwork
	TARGET_LIBDIR = $(MODE_DIR)/interwork
endif



# Package definitions
PKG_NAME	= target
DATE		= $$(date +%Y%m%d)

# Tool definitions
CC      = arm-elf-gcc
CPP     = arm-elf-g++
LD      = arm-elf-ld -v
AR      = arm-elf-ar
AS      = arm-elf-as
CP      = arm-elf-objcopy
OD		= arm-elf-objdump
RM		= rm
TAR		= tar
COPY	= cp

#don't use -O0 on GCC 4.4.0 (uip_arp resolution will fail)

CFLAGS  = -I./ -I../ -c -W  -Os -mcpu=arm7tdmi -mthumb-interwork -m$(MODE)
CFLAGS_ARM  =  -I./ -I../ -c -W  -Os -mcpu=arm7tdmi -mthumb-interwork -marm
ARFLAGS = -rcs

RTOS_UIP = .

LIBSRCS = $(RTOS_UIP)/clock.c $(RTOS_UIP)/lib/memb.c  \
		  $(RTOS_UIP)/uip/uip-neighbor.c	$(RTOS_UIP)/uip/uiplib.c	\
		  $(RTOS_UIP)/uip/uip_timer.c $(RTOS_UIP)/uip/uip-fw.c $(RTOS_UIP)/uip/uip_arp.c	\
		  $(RTOS_UIP)/uip/uip_arch.c $(RTOS_UIP)/uip/uip.c $(RTOS_UIP)/uip/psock.c \
		  $(RTOS_UIP)/apps/dhcpc/dhcpc.c
		  
# $(RTOS_UIP)/uiptask.c 

LIBOBJS = $(LIBSRCS:.c=.o)

LIBSRCS_ARM = $(RTOS_UIP)/hardware/enc28j60.c $(RTOS_UIP)/hardware/enc28j60_isr.c 
LIBOBJS_ARM = $(LIBSRCS_ARM:.c=.o)


all: lib 

$(LIBOBJS) : %.o : %.c
	$(CC) $(CFLAGS)  $< -c -o $@ 
    
$(LIBOBJS_ARM) : %.o : %.c
	$(CC) $(CFLAGS_ARM)  $< -c -o $@ 

dir: $(ESPARDINO)/arm-elf/include/uip

$(ESPARDINO)/arm-elf/include/uip:
	mkdir $@

install: all dir
	$(COPY) -f $(LIBNAME).a "$(ESPARDINO)/arm-elf/lib/$(TARGET_LIBDIR)/lib$(LIBNAME).a"
	$(COPY) -f uip/*.h "$(ESPARDINO)/arm-elf/include/uip"

clean:
	$(RM) -f $(LIBNAME).a $(LIBOBJS) $(LIBOBJS_ARM) .depend

program:
	c:/python26/python docbuild.py

# build lib
lib: $(LIBNAME).a 

$(LIBNAME).a: $(LIBOBJS) $(LIBOBJS_ARM)
	$(AR) $(ARFLAGS) $@ $^

# Builds release tar file
dist: clean
	cd .. && $(TAR) --exclude={CVS,cvs,.svn} -cvzf $(PKG_NAME)-$(DATE).tar.gz $(PKG_NAME)

# recompile if the Makefile changes
$(LIBOBJS): Makefile


# dependency checking
depend: $(LIBSRCS)
	$(CC) $(CFLAGS) -MM $^ > .depend || rm -f .depend

# phony targets
.PHONY: all clean examples depend

-include .depend
