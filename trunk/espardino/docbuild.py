import re
import ftplib
import os.path
import sys, os
from config import info,directories
import hashlib


DEBUG = False

DOC_PATH = "./docgen/"


def md5(fileName, excludeLine="", includeLine=""):
    """Compute md5 hash of the specified file"""
    m = hashlib.md5()
    try:
        fd = open(fileName,"rb")
    except IOError:
        print "Unable to open the file in readmode:", filename
        return
    content = fd.readlines()
    fd.close()
    for eachLine in content:
        if excludeLine and eachLine.startswith(excludeLine):
            continue
        m.update(eachLine)
    m.update(includeLine)
    return m.hexdigest()


def putfile(file_local,file,site,dir,user=(), verbose = True):
	
	md5_sum = md5(file_local)
	md5_file = file_local+".uploaded.md5"
	last_md5 ="***"
	
	if os.path.exists(md5_file):
		f = open(md5_file,'r')
		
		last_md5 = f.readlines()[0].rstrip("\n")
		f.close()
	
	if md5_sum == last_md5:
		print "uploading=> not needed, file output unchanged"
		return
	
	local = open(file_local,'rb')
	remote = ftplib.FTP(site)
	remote.login(*user)
	remote.cwd(dir)
	try:
		remote.delete(file)
	except:
		pass
		
	remote.storbinary('STOR '+file,local,1024)
	remote.quit()
	local.close()
	
	f = open(md5_file,'w')
	f.write(md5_sum+"\n")
	f.close()


    






f_out = None
f_name = None
f_path = None

def f_close():
	
	global f_out
	
	if f_out==None:
		return
		
	f_out.close()
	f_out = None
	
	site = 'wiki.espardino.com'            #hard-coded
	
	(dir, filename) = os.path.split(f_name.rstrip('\n').rstrip('\r').rstrip(' '))

	
	print "uploading=> dir:",dir, " filename:",filename
	
	putfile(f_name, filename, site, dir, user=info)

def issue_command(command,info):
	
	global f_out,f_name,DEBUG
	
	if DEBUG:
		print "issue_command: ",command,info
	
	if command=="FILE":
		
		if f_out!=None:
			f_close()	
		f_name = info.rstrip("\n").rstrip(" ")
		f_out = open(f_name,'w')
		
		
		print "created output file ",f_name
		
		libname = f_name.replace("/",":").replace(".txt","")
		f_out.write("~~DISCUSSION~~\n");
		f_out.write(":!: this information is automatically generated, for comments please use the discussion box at bottom\n\n");
		#[[")
		#f_out.write(libname+":"+libname+"-comments")
		#f_out.write("]]\n\n");
		
	elif command=="INTRO":
		f_out.write(info);
		
	elif command=="RESOURCES":
		f_out.write("**Used resources:** "+info+"\n")
		
	elif command=="USAGE":
		f_out.write("====== "+info+" ======\n")
	
	elif command=="SECTION":
		f_out.write("====== "+info+" ======\n")
	
	elif command=="FUN":			
		f_out.write("  * //**"+info+"**//\n")
	
	else:
		print "!!!!!!!!!! UNEXPECTED COMMAND '%s'"%(command)
	
		
	
def issue_command_section(command,info,section):

	global DEBUG
	
	if DEBUG:
		print "issue_command_section: ",command,section
	
	
	if (command=="USAGE" or command=="SECTION") and section=="INCLUDE":
		f = open(f_path+"/"+info,'r')
		data = f.read()
		f.close()
		f_out.write(data+"\n")
	elif command=="FUN" and section=="ACTION":
		f_out.write("    * **Action:** ")
	
	elif command=="FUN" and section=="RESULT":
		f_out.write("\n    * **Result:** ")
		
	elif command=="FUN" and section=="EXTRA":
		f_out.write("\n\n");
	else:
		print "!!!!!!!!!! UNEXPECTED SECTION '%s' FOR COMMAND '%s'"%(section,command)
		    

	

	
def issue_command_end(command,info):
	
	global DEBUG
	
	if DEBUG:
		print "issue_command_end: ",command
	
	if command=="FUN":
		f_out.write("\n\n");
	
	
def issue_command_data(command,info,section,line):	

	global DEBUG
	
	if DEBUG:
		print "issue_command_data: ", line
	
	if command=="INTRO":
		f_out.write(line+"\n");
		
	elif command=="RESOURCES":
		f_out.write(line+"\n");
	
	elif command=="USAGE":
		f_out.write(line+"\n");
		
	elif command=="SECTION":
		f_out.write(line+"\n");
	
	elif command=="FUN":
		if section=="" or section=="EXTRA":
			f_out.write(line+"\n");
		elif section=="ACTION":
			f_out.write(line)
		elif section=="RESULT":
			f_out.write(line);
		else:
			print "!!!!!!!!!!! UNKOWN FUN SECTION (%s) DATA"%section
		
	else:
		print "!!!!!!!!!! UNEXPECTED DATA '%s' FOR COMMAND '%s'"%(line,command)
		
		
		
	
	
	
	
def scan_file(filename):

	global f_path
	
	f = open(filename,'r')
	file_content = f.readlines()
	f.close()
	
	
	(f_path, fnme) = os.path.split(filename.rstrip('\n').rstrip('\r').rstrip(' '))
	
	comments = []
	started = False
	ended = False
	state ="WAIT_START"
	command = ""
	info = ""
	section = ""
	
	for line in file_content:
		
		if state == "WAIT_START":
			if len(line)<5:
				continue
				
			if line[0:4]=="/*==" and line[0:6]!="/*====":
				contents = line[4:]
				command = contents.split(" ")[0].rstrip('\n').rstrip('\r')
				info = contents[len(command)+1:].rstrip('\n').rstrip('\r').rstrip(' ').rstrip('=')
				
				if not started:
					started = True
					print "=======> "+filename +" <=========="
					
			
				if info.endswith("*/"):
					
					issue_command(command,info[:-2])
					issue_command_end(command,"")
					section = ""
				else:
					issue_command(command,info)
					state = "WAIT_DATA"
					section = ""
				
				
		elif state == "WAIT_DATA":
			if line[0:2]=="==":
				section = (line[2:].rstrip('\n').rstrip('\r').split(" "))[0]
				info = line.split(" ")
				if len(info)>1:
					info = " ".join(info[1:]).rstrip("\n").rstrip('\r');
				else:
					info = ""
					
				issue_command_section(command,info,section)
				
			elif line[0:2]=="*/":
				issue_command_end(command,info)
				state = "WAIT_START"
			else:
				issue_command_data(command,info,section,line.rstrip('\n').rstrip('\r'))
				
			
	if filename.endswith(".h") or filename.endswith(".hpp"):
		file2 = filename.replace(".h",".c")
		if os.path.exists(file2):
			scan_file(file2)
		
		
			
			
	
				
				
	

def search_files(c_dir):
		for f in os.listdir(c_dir):
			c_file = c_dir+"/"+f
			if os.path.isdir(c_file):
				search_files(c_file)
				
			else:
				if f.endswith(".h") or f.endswith(".hpp") or f.endswith(".wdoc"):

					scan_file(c_file)
					f_close()
					
				


for c_dir in directories:
	search_files(c_dir)
	
	
	
	
