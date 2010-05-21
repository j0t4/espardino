LIBNAME	= micro214x

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
CPP      = arm-elf-g++
LD      = arm-elf-ld -v
AR      = arm-elf-ar
AS      = arm-elf-as
CP      = arm-elf-objcopy
OD		= arm-elf-objdump
RM		= rm
TAR		= tar
COPY	= cp

CFLAGS  = -I. -I.. -c -W -Wall -Os -mcpu=arm7tdmi -mthumb-interwork -m$(MODE)
CFLAGS_ARM  = -I. -I.. -c -W -Wall -Os -mcpu=arm7tdmi -mthumb-interwork -marm
ASFLAGS = -mcpu=arm7tdmi -mthumb-interwork -I.  -x assembler-with-cpp
ARFLAGS = -rcs

LIBCPPSRCS = nanoc++.cpp digital_bus.cpp pwm_c++.cpp ticker.cpp
LIBCPPOBJS = $(LIBCPPSRCS:.cpp=.o)

LIBSRCS = leds.c pwm.c efsutils.c _sbrk_r.c adc.c sysclocks.c iolib.c rtc.c delay.c dabt_handler.c buffer.c serial_device.c 
LIBOBJS = $(LIBSRCS:.c=.o)

LIBSRCS_ARM = arm_irqs.c i2c_device.c pwm_in.c miniprintf.c serial_device_ARM_handler.c
LIBOBJS_ARM = $(LIBSRCS_ARM:.c=.o)

LIBSRCS_ASM = dabt_handler_asm.s
LIBOBJS_ASM = $(LIBSRCS_ASM:.s=.o)


all: lib  

$(LIBOBJS_ASM) : %.o : %.s
	$(CC) $(ASFLAGS) $< -c

$(LIBOBJS) : %.o : %.c
	$(CC) $(CFLAGS)  $< -c
    
$(LIBCPPOBJS) : %.o : %.cpp
	$(CPP) $(CFLAGS)  $< -c

$(LIBOBJS_ARM) : %.o : %.c
	$(CC) $(CFLAGS_ARM)  $< -c

install: all
	$(COPY) -f $(LIBNAME).a "$(ESPARDINO)/arm-elf/lib/$(TARGET_LIBDIR)/lib$(LIBNAME).a"
	$(COPY) -f *.h "$(ESPARDINO)/arm-elf/include/"

clean:
	$(RM) -f $(LIBNAME).a $(LIBOBJS) $(LIBOBJS_ARM) $(LIBCPPOBJS) .depend

program:
	c:/python26/python docbuild.py

# build lib
lib: $(LIBNAME).a

$(LIBNAME).a: $(LIBOBJS) $(LIBOBJS_ARM) $(LIBOBJS_ASM) $(LIBCPPOBJS)
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
