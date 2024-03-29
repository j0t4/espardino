LIBNAME	= micro214x
OPT=-O2

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
MKDIR = mkdir

CFLAGS  =  -g -I. -I.. -c -W -Wall $(OPT) -mcpu=arm7tdmi -mthumb-interwork -m$(MODE)
CFLAGS_ARM  = -g -I. -I.. -c -W -Wall $(OPT) -mcpu=arm7tdmi -mthumb-interwork -marm
ASFLAGS = -g -mcpu=arm7tdmi -mthumb-interwork -I.  -x assembler-with-cpp
ARFLAGS = -rcs

LIBCPPSRCS = gfx/bitmaplcd.cpp gfx/rle.cpp gfx/ssd1308.cpp nanoc++.cpp digital_bus.cpp pwm_c++.cpp hasprintf.cpp serial.cpp dac.cpp leds_c++.cpp spi_c++.cpp
LIBCPPOBJS = $(LIBCPPSRCS:.cpp=.o)

LIBCPPSRCS_ARM = ticker.cpp
LIBCPPOBJS_ARM = $(LIBCPPSRCS_ARM:.cpp=.o)


LIBSRCS = spi.c leds.c pwm.c efsutils.c _sbrk_r.c adc.c sysclocks.c iolib.c rtc.c delay.c dabt_handler.c buffer.c serial_device.c 
LIBOBJS = $(LIBSRCS:.c=.o)

LIBSRCS_ARM = arm_irqs.c i2c_device.c pwm_in.c miniprintf.c serial_device_ARM_handler.c
LIBOBJS_ARM = $(LIBSRCS_ARM:.c=.o)

LIBSRCS_ASM = dabt_handler_asm.s
LIBOBJS_ASM = $(LIBSRCS_ASM:.s=.o)


all: lib  

$(LIBOBJS_ASM) : %.o : %.s
	$(CC) $(ASFLAGS) $< -c -o $@

$(LIBOBJS) : %.o : %.c
	$(CC) $(CFLAGS)  $< -c -o $@
    
$(LIBCPPOBJS) : %.o : %.cpp
	$(CPP) -fno-rtti $(CFLAGS)  $< -c -o $@

$(LIBCPPOBJS_ARM) : %.o : %.cpp
	$(CC) $(CFLAGS_ARM)  $< -c -o $@


$(LIBOBJS_ARM) : %.o : %.c
	$(CC) $(CFLAGS_ARM)  $< -c -o $@

install: all
	$(COPY) -f $(LIBNAME).a "$(ESPARDINO)/arm-elf/lib/$(TARGET_LIBDIR)/lib$(LIBNAME).a"
	$(COPY) -f *.h "$(ESPARDINO)/arm-elf/include/"
	$(MKDIR) -p "$(ESPARDINO)/arm-elf/include/gfx"
	$(COPY) -rf gfx/*.hpp "$(ESPARDINO)/arm-elf/include/gfx/"
	$(COPY) -rf gfx/*.h "$(ESPARDINO)/arm-elf/include/gfx/"
	$(COPY) -f *.hpp "$(ESPARDINO)/arm-elf/include/"
	$(COPY) -f mon48.bin "$(ESPARDINO)/arm-elf/monitor/espardino_monitor_lpc2148.bin"

clean:
	$(RM) -f $(LIBNAME).a $(LIBOBJS) $(LIBOBJS_ARM) $(LIBCPPOBJS) $(LIBCPPOBJS_ARM) .depend

program:
	c:/python26/python docbuild.py

# build lib
lib: $(LIBNAME).a

$(LIBNAME).a: $(LIBOBJS) $(LIBOBJS_ARM) $(LIBOBJS_ASM) $(LIBCPPOBJS) $(LIBCPPOBJS_ARM)
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
