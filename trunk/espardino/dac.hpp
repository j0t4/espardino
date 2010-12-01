#ifndef __DAC_H
#define __DAC_H

#include <LPC214x.h>
#include <ticker.hpp>

class DAC
{
	Ticker ticker;
	bool ticker_started;
	signed short* buffer;
	int buffer_size;
	int buffer_p;
	int buffer_p_play;
	int underruns;
public:
	static DAC* singleton;
	
	DAC();
	~DAC();
	void write(signed short data) { DACR = (data+0x8000); }
	
	void tickPlay(void);
	
	int setTicker(int frequency,int buffer_size=256);
	
	int add(signed short data);
	int add(signed short *data, int len);
	int space();
	
};

#endif