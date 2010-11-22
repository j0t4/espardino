#ifndef __RLE_H
#define __RLE_H

#include <micro214x.h>

class RLE
{
	private:
	  unsigned char *p;
		unsigned char last_c;
		unsigned int rep;
		unsigned char state;

	public:
		RLE(char *compressed);
		int get();
		
};

#endif
