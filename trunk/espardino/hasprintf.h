#ifndef __HASPRINTF_H_
#define __HASPRINTF_H_

class HasPrintf
{
private:
	int printfwarning();
public:
	int printf(char *fmt,...);
	int printf(const char *fmt,...);
virtual int write(char c)=0;
};

#endif

