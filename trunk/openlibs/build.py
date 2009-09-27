#!/bin/env python
import subprocess
import os
import sys

LIBRARY = ""
DIRECTORY="."
MODES = ["thumb","arm"]
INTERWORKING = ["yes","no"]

MAKE_CMDLINE_FMT = "make %(what)s MODE=%(mode)s INTERWORK=%(interwork)s"

def print_banner(lib,mode,interwork):
	
	print ""	
	print "=================================================================================="
	print ""
	print "	   building ",lib," processor mode=",mode," interworking=",interwork
	print ""
	print "=================================================================================="

def run_make(what,mode,interwork):
	MAKE_CMDLINE = MAKE_CMDLINE_FMT % {"what":what,"mode":mode,"interwork":interwork}
	print ""
	print MAKE_CMDLINE
	
	result = subprocess.call(MAKE_CMDLINE,shell=True)
	if result!=0:
		print "[error doing",MAKE_CMDLINE,"]"
		sys.exit(1)	


def build_version(mode,interwork):

	print_banner(LIBRARY,mode,interwork)
	run_make("all",mode,interwork)
	run_make("install",mode,interwork)
	run_make("clean",mode,interwork)

	
try:
	LIBRARY = sys.argv[1]
	DIRECTORY = sys.argv[2]
except:
	pass	

cwd_save = os.getcwd()
os.chdir(DIRECTORY)
	
for mode in MODES:
	for interworking in INTERWORKING:
		build_version(mode,interworking)

	
os.chdir(cwd_save)	
	
	




