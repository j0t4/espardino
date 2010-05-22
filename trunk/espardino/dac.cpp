#include <dac.h>
#include <stdlib.h>

DAC* DAC::singleton=NULL;
	
DAC::DAC() 
{ 
		underruns = 0;
		ticker_started= false; 
		singleton=this; 
		// Setup P0.25 as analog output
		PINSEL1 = (PINSEL1&(~(3<<18)))|(2<<18);
		buffer = NULL;

};

DAC::~DAC() 
{ 
	if (ticker_started) ticker.stop(); 
	if (buffer) free(buffer);
}

void DAC_tickPlayer()
{
	if (DAC::singleton) DAC::singleton->tickPlay();
}


void DAC::tickPlay(void)
{
		write(buffer[buffer_p_play]);
		if (buffer_p_play!=buffer_p) buffer_p_play++;
			
		if (buffer_p_play>=buffer_size) buffer_p_play=0;
}

int DAC::add(signed short data)
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

int DAC::add(signed short *data, int len)
{
	int old_len=len;
	while (len--) add(*data++);
	return old_len;
}

int DAC::setTicker(int frequency,int buffer_size)
{
	/* setup the buffer and pointers */
	buffer = (signed short *)malloc(buffer_size*sizeof(signed short));
	if (!buffer) return -1;
	this->buffer_size=buffer_size;
	buffer_p = 0;
	buffer_p_play=0;
	
	/* setup the ticker to call DAC_tickPlayer */
	ticker.attach(DAC_tickPlayer);
	ticker.setFrequency(frequency);
	ticker.start();
	ticker_started = true;
	return buffer_size;
}
	
		
int DAC::space()
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
	

