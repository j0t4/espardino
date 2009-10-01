LIBNAME	= micro214x

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

CFLAGS  = -I./ -I../ -c -W -Wall -Os -mcpu=arm7tdmi -mthumb-interwork -mthumb
CFLAGS_ARM  = -I./ -I../ -c -W -Wall -Os -mcpu=arm7tdmi -mthumb-interwork -marm
ASFLAGS = -mcpu=arm7tdmi -mthumb-interwork -I.  -x assembler-with-cpp
ARFLAGS = -rcs

LIBSRCS = leds.c pwm.c efsutils.c _sbrk_r.c adc.c sysclocks.c iolib.c rtc.c delay.c dabt_handler.c
LIBOBJS = $(LIBSRCS:.c=.o)

LIBSRCS_ARM = arm_irqs.c i2c_device.c pwm_in.c miniprintf.c
LIBOBJS_ARM = $(LIBSRCS_ARM:.c=.o)

LIBSRCS_ASM = dabt_handler_asm.s
LIBOBJS_ASM = $(LIBSRCS_ASM:.s=.o)


all: comp_ARM comp lib  

$(LIBOBJS_ASM) : %.o : %.s
	$(CC) $(ASFLAGS) $< -c

$(LIBOBJS) : %.o : %.c
	$(CC) $(CFLAGS)  $< -c
    
$(LIBOBJS_ARM) : %.o : %.c
	$(CC) $(CFLAGS_ARM)  $< -c


comp_ARM: $(LIBSRCS_ARM)
	$(CC) $(CFLAGS_ARM)  $< -c  

comp: $(LIBSRCS)
	$(CC) $(CFLAGS)  $< -c  	

install: all
	$(COPY) -f $(LIBNAME).a "$(ESPARDINO)/arm-elf/lib/lib$(LIBNAME).a"
	$(COPY) -f *.h "$(ESPARDINO)/arm-elf/include/"

clean:
	$(RM) -f $(LIBNAME).a $(LIBOBJS) $(LIBOBJS_ARM) .depend

program:
	c:/python26/python docbuild.py

# build lib
lib: $(LIBNAME).a

$(LIBNAME).a: $(LIBOBJS) $(LIBOBJS_ARM) $(LIBOBJS_ASM)
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
