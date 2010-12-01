#include <dac.hpp>
#include <stdlib.h>

DACBuffer* DACBuffer::singleton=NULL;
	
DACBuffer::DACBuffer() 
{ 
		underruns = 0;
		ticker_started= false; 
		singleton=this; 
		// Setup P0.25 as analog output
		PINSEL1 = (PINSEL1&(~(3<<18)))|(2<<18);
		buffer = NULL;

};

DACBuffer::~DACBuffer() 
{ 
	if (ticker_started) ticker.stop(); 
	if (buffer) free(buffer);
}

void DACBuffer_tickPlayer()
{
	if (DACBuffer::singleton) DACBuffer::singleton->tickPlay();
}


void DACBuffer::tickPlay(void)
{
		write(buffer[buffer_p_play]);
		if (buffer_p_play!=buffer_p) buffer_p_play++;
			
		if (buffer_p_play>=buffer_size) buffer_p_play=0;
}

int DACBuffer::add(signed short data)
{
	int next_p;
	
	do /* wait until there is enough space */
	{
		next_p = buffer_p+1;
		if (next_p>=buffer_size) next_p=0;
	} while (next_p==buffer_p_play);
	
	buffer[buffer_p]=data;
	return 1;
}

int DACBuffer::add(signed short *data, int len)
{
	int old_len=len;
	while (len--) add(*data++);
	return old_len;
}

int DACBuffer::setFrequency(int frequency,int buffer_size)
{
	/* setup the buffer and pointers */
	buffer = (signed short *)malloc(buffer_size*sizeof(signed short));
	if (!buffer) return -1;
	this->buffer_size=buffer_size;
	buffer_p = 0;
	buffer_p_play=0;
	
	/* setup the ticker to call DACBuffer_tickPlayer */
	ticker.attach(DACBuffer_tickPlayer);
	ticker.setFrequency(frequency);
	ticker.start();
	ticker_started = true;
	return buffer_size;
}
	
		
int DACBuffer::space()
{
	
	if (buffer_p_play<buffer_p)
		{
			return ((buffer_p+buffer_size)-buffer_p_play);
		}
	else
		{
			return (buffer_p - buffer_p_play);
		}
}
	

