#
# www.espardino.com Espardino Makefile
#
# On command line:
#
# make all = Make software.
# make clean = Clean out built project files.
# make program = Download the hex file to the device
#

# c-only (16bit compressed thumb mode)
SRC = lpc2000_spi.c blockdev_sd.c  main.c  msc_bot.c  msc_scsi.c

# c-only (32bit arm mode)
SRCARM = 		

# c++ (16 bit compressed thumb mode)
CPPSRC = 

# c++ (32 bit arm mode)
CPPSRCARM = 

# Assembler (16 bit compressed thumb mode)
ASRC = 

# Assembler (32 bit arm mode) always capital .S at the end
ASRCARM = 

# System libraries to be linked with
MY_LIBS = -llpcusb 

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization (if you find problems). s = optimize for size.
OPT = 1

include $(ESPARDINO)/src/espardino/makefiles/basemake.mk
