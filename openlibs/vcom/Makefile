LIBNAME	= vcom

ifeq ($(MODE),thumb)
	MODE_DIR = thumb
        MODE=thumb
else
	MODE_DIR = .
        MODE=arm
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
LD      = arm-elf-ld -v
AR      = arm-elf-ar
AS      = arm-elf-as
CP      = arm-elf-objcopy
OD		= arm-elf-objdump
RM		= rm
TAR		= tar
COPY    = cp

CFLAGS  = -I./ -I../ -c -W -Wall -Os -mcpu=arm7tdmi -mthumb-interwork -m$(MODE)
CFLAGS_ARM  = -I./ -I../ -c -W -Wall -Os -mcpu=arm7tdmi -mthumb-interwork -marm
ARFLAGS = -rcs

LIBSRCS = usbhw_lpc.c usbcontrol.c usbstdreq.c usbinit.c  serial_fifo.c armVIC.c
LIBSRCS_ARM = armVIC.c main_serial.c

LIBOBJS = $(LIBSRCS:.c=.o)
LIBOBJS_ARM = $(LIBSRCS_ARM:.c=.o)

all: depend comp lib

$(LIBOBJS) : %.o : %.c
	$(CC) $(CFLAGS)  $< -c
    
$(LIBOBJS_ARM) : %.o : %.c
	$(CC) $(CFLAGS_ARM)  $< -c



comp: $(LIBSRCS) $(LIBSRCS_ARM)
	$(CC) $(CFLAGS)  $< -c  	



clean:
	$(RM) -f $(LIBNAME).a $(LIBOBJS) $(LIBOBJS_ARM) .depend


# build lib
lib: $(LIBNAME).a

$(LIBNAME).a: $(LIBOBJS) $(LIBOBJS_ARM)
	$(AR) $(ARFLAGS) $@ $^

install: all
	$(COPY) -f $(LIBNAME).a "$(ESPARDINO)/arm-elf/lib/$(TARGET_LIBDIR)/lib$(LIBNAME).a"
	$(COPY) -f vcom.h "$(ESPARDINO)/arm-elf/include/"

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
