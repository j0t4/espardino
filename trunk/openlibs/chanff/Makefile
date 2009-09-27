LIBNAME	= ffs
MODE = thumb

ifeq ($(MODE),thumb)
	MODE_DIR = thumb
else
	MODE_DIR = .
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

CFLAGS  = -I./ -I../ -c -W -Wall -Os -mcpu=arm7tdmi $(INTERWORK_PARAM) -m$(MODE) 
ARFLAGS = -rcs

LIBSRCS = fattime.c ff.c sd_spi_lpc2000.c option/ccsbcs.c
LIBOBJS = $(LIBSRCS:.c=.o)

all: depend lib

clean:
	$(RM) -f $(LIBNAME).a $(LIBOBJS) .depend


# build lib
lib: $(LIBNAME).a

$(LIBNAME).a: $(LIBOBJS)
	$(AR) $(ARFLAGS) $@ $^

install: all
	$(COPY) -f $(LIBNAME).a "$(ESPARDINO)/arm-elf/lib/$(TARGET_LIBDIR)/lib$(LIBNAME).a"
	$(COPY) -f ff.h "$(ESPARDINO)/arm-elf/include/"
	$(COPY) -f integer.h "$(ESPARDINO)/arm-elf/include/"

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
