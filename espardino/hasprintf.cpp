#include <hasprintf.hpp>
#include <micro214x.h>

int hasprintf_locked=false;
HasPrintf *current;

extern "C" int xprintf_print(char **out, int *varg);

int hasprintf_write(int c)
{
	return current->write(c);
}
int HasPrintf::printfwarning()
{
	char *p=(char *)".printf won't work from RAM\r\n";
	while(*p) write(*p++);
	return -1;
	
}
int HasPrintf::printf(char *fmt,...)
{
	 int len;
	 printFuncPt out = hasprintf_write;
	 
	 if (((int)out)>0x10000000) 
	 	{
	 		return printfwarning(); /* this printf won't work if we're in ram */
	 	}
	 
	 int *varg = (int *)(&fmt);
	 
	 if (hasprintf_locked) return -1;	
	 hasprintf_locked=true;
	 current = this;
	 len =  xprintf_print((char**)&out, varg);
	 hasprintf_locked=false;
	 
	 return len;
}

int HasPrintf::printf(const char *fmt,...)
{
	 
	 int len;
	 printFuncPt out = hasprintf_write;
	 
	 if (((int)out)>0x10000000) 
	 	{
	 		return printfwarning(); /* this printf won't work if we're in ram */
	 	}
	 
	 int *varg = (int *)(&fmt);
	 
	 if (hasprintf_locked) return -1;	
	 hasprintf_locked=true;
	 current = this;
	 len =  xprintf_print((char**)&out, varg);
	 hasprintf_locked=false;
	 
	 return len;
}