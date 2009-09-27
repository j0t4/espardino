

SRC_FILES=clock.c uiptask.c

SUBDIRS=apps hardware lib uip

#
# Define all object files.
#
ARM_OBJ = $(SRC_FILES:.c=.o)

.PHONY: all
ifeq ($(STARTEDATTOP),true)
all : subdirs $(ARM_OBJ)
else
all :
	@echo "Project must be rebuilt from top level"
endif

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

$(ARM_OBJ) : %.o : %.c Makefile .depend
	$(CC) -c $(CFLAGS) -Wno-cast-align $< -o $@
	$(AR) rc $(COMMON)/common.a $@

#
#  The .depend files contains the list of header files that the
#  various source files depend on.  By doing this, we'll only
#  rebuild the .o's that are affected by header files changing.
#
.depend:
	$(CC) $(CFLAGS) -M $(SRC_FILES) -o .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
