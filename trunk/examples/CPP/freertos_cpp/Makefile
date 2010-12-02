#
# www.espardino.com Espardino Makefile
#
# make all = Make software.
# make clean = Clean out built project files.
# make program = Download the hex file to the device, using lpc21isp

# c-only (16bit compressed thumb mode)
SRC = 

# c-only (32bit arm mode)
SRCARM = 

# c++ (16 bit compressed thumb mode)
CPPSRC = main.cpp 

# c++ (32 bit arm mode)
CPPSRCARM = 

# Assembler (16 bit compressed thumb mode)
ASRC = 

# Assembler (32 bit arm mode) always capital .S at the end
ASRCARM = 

# System libraries to be linked with
MY_LIBS = -lvcom_rtos -lfreertos

# override the default espardino startup.S and use freertos specific one
MY_STARTUP = $(ESPARDINO)/src/espardino/startup_freertos.S 

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization (if you find problems). s = optimize for size.
OPT = 1

include $(ESPARDINO)/src/espardino/makefiles/basemake.mk

