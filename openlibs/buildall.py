#!/usr/bin/env python

from subprocess import call
import sys 

exec_list = [
	"python build.py chanff chanff",
	"python build.py efs efs/src",
	"python build.py FreeRTOS FreeRTOS",
	"python build.py lua lua",
	"python build.py vcom_rtos vcom_rtos",
	"python build.py vcom vcom",
	"python build.py lpcusb lpcusb"]



for cmd in exec_list:
	result = call(cmd,shell=True)
	if result!=0:
		print "[ failed ",cmd,"]"
		sys.exit(1)


