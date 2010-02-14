#
# www.espardino.com Espardino Makefile
#
#
# On command line:
#
# make all = Make software.
# make clean = Clean out built project files.
# make program = Download the hex file to the device
#


# c-only (16bit compressed thumb mode)
SRC = main.c

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
MY_LIBS = -lffs -lvcom 


# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization (if you find problems). s = optimize for size.
OPT = s

include $(ESPARDINO)/src/espardino/makefiles/basemake.mk
