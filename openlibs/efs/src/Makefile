LIBNAME	= efs
MODE = thumb

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
COPY = cp

CFLAGS  = -I./ -I../conf -I../inc -Isrc -c -W -Wall -Os -mcpu=arm7tdmi -mthumb-interwork -m$(MODE)
ARFLAGS = -rcs

LIBSRCS = efs.c plibc.c disc.c partition.c time.c fs.c fat.c file.c dir.c mkfs.c debug.c ioman.c ui.c extract.c ls.c interfaces/sd.c interfaces/lpc2000_spi.c interfaces/lpc2000_dbg_printf.c
LIBOBJS = $(LIBSRCS:.c=.o)

all: depend lib

clean:
	$(RM) -f $(LIBNAME).a $(LIBOBJS) .depend


# build lib
lib: $(LIBNAME).a

$(LIBNAME).a: $(LIBOBJS)
	$(AR) $(ARFLAGS) $@ $^


install: all
	$(COPY) -f $(LIBNAME).a "$(ESPARDINO)/arm-elf/lib/lib$(LIBNAME).a"
	$(COPY) -f ../inc/*.h "$(ESPARDINO)/arm-elf/include/$(LIBNAME)/"
	
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
