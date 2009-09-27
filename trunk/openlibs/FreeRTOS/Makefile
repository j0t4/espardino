LIBNAME	= freertos

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

RTOS_DIR=.
RTOS_SOURCE_DIR=$(RTOS_DIR)/Source




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

CFLAGS  = -I$(RTOS_SOURCE_DIR)/include  -I. -I../ -I../..-c -W -Wall -Os -mcpu=arm7tdmi $(INTERWORK_PARAM) -m$(MODE) 
CFLAGS_ARM  = -I$(RTOS_SOURCE_DIR)/include  -I. -I../ -I../.. -c -W -Wall -Os -mcpu=arm7tdmi $(INTERWORK_PARAM) -marm
ARFLAGS = -rcs

LIBSRCS =  $(RTOS_SOURCE_DIR)/tasks.c $(RTOS_SOURCE_DIR)/queue.c $(RTOS_SOURCE_DIR)/list.c \
		 $(RTOS_SOURCE_DIR)/portable/MemMang/heap_3.c $(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC2000/port.c 



LIBOBJS = $(LIBSRCS:.c=.o)

LIBSRCS_ARM = $(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC2000/portISR.c 	
LIBOBJS_ARM = $(LIBSRCS_ARM:.c=.o)

all: depend comp lib


$(LIBOBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $(CONLYFLAGS) $< -o $@ 

$(LIBOBJS_ARM) : %.o : %.c
	$(CC) -c $(CFLAGS_ARM) $(CONLYFLAGS) $< -o $@ 

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
	$(COPY) Source/include/*.* includes/
	$(COPY) Source/portable/GCC/ARM7_LPC2000/portmacro.h includes/portable.h
	$(COPY) FreeRTOSConfig.h includes/
	
	$(COPY) -f includes/* "$(ESPARDINO)/arm-elf/include/freertos/"
	

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

-include .depend/
