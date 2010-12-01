#ifndef __SPI_CPP_H
#define __SPI_CPP_H

#include "iolib.h"

#define SPI0_MOSI P0_6
#define SPI0_MISO P0_5
#define SPI0_SCK  P0_4

#define SPI1_MOSI P0_19
#define SPI1_MISO P0_18
#define SPI1_SCK  P0_17

class Spi 
{
  private:
	int m_hwspi_port;
	int m_bps;
	int m_CS_io[4];
	int m_MOSI_io,m_MISO_io,m_SCK_io;
	
	void init();

  public:
	Spi();
	Spi(int MOSI_pin, int MISO_pin, int SCK_pin, int CS_pin0, int CS_pin1=NO_PIN, int CS_pin2=NO_PIN, int CS_pin3=NO_PIN);
	~Spi();
	
	void attach (int MOSI_pin, int MISO_pin, int SCK_pin, int CS_pin0, int CS_pin1=NO_PIN, int CS_pin2=NO_PIN, int CS_pin3=NO_PIN );

	/* send&receive functions: character */
	void waitData();
	unsigned int xfer(unsigned int data);
	
	/* set bits per second */
	void setBps(int bps);
	int getBps() { return m_bps; }
	
	void chipSelect(int cs);
	void deSelect();   
};

#endif
