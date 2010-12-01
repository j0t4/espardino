#include "spi_c++.hpp"
#include "micro214x.h"

Spi::Spi()
{
	m_bps = 400000;		
	m_MOSI_io 	= NO_PIN;
	m_MISO_io 	= NO_PIN;
	m_CS_io[0]  = NO_PIN;
	m_CS_io[1]  = NO_PIN;
	m_CS_io[2]  = NO_PIN;
	m_CS_io[3]  = NO_PIN;
	
}

Spi::Spi(int MOSI_pin, int MISO_pin, int SCK_pin, int CS_pin0, int CS_pin1, int CS_pin2, int CS_pin3)
{
	m_bps = 400000;		
	attach(MOSI_pin, MISO_pin, SCK_pin, CS_pin0, CS_pin1, CS_pin2, CS_pin3);
}


Spi::~Spi()
{
}

void Spi::init()
{
		if (m_MOSI_io==P0_6 && m_MISO_io==P0_5 && m_SCK_io == P0_4)
		{
				m_hwspi_port = SPI0;
		}
		else
		if (m_MOSI_io==P0_19 && m_MISO_io==P0_18 && m_SCK_io == P0_17)
		{
				m_hwspi_port = SPI1;
		}
		else
		{
				m_hwspi_port = -1; /* software SPI, still not supported */
		}		
		
		if (m_hwspi_port>=0)
		{
			SPI_init(m_hwspi_port,SPI_8BIT,m_bps);
		}		
		
}

void Spi::setBps(int bps)
{
	m_bps = bps;
	init();
}		

	
void Spi::chipSelect(int cs)
{
	int i;
	
	for (i=0;i<4;i++)
	{
		if (m_CS_io[i]==NO_PIN) continue;
			
		if (i!=cs) IO_output_high(m_CS_io[i]);
		else			 IO_output_low(m_CS_io[i]);
	}
	
}

void Spi::deSelect()
{
		waitData();
		chipSelect(-1);
}


void Spi::waitData()
{
		if (m_hwspi_port>=0)
		{
			SPI_wait(m_hwspi_port);
		}		
}

unsigned int Spi::xfer(unsigned int data)
{
		if (m_hwspi_port>=0)
		{
			return SPI_xfer(m_hwspi_port,data);
		}		
		
		return 0xfffff;
}




void Spi::attach(int MOSI_pin, int MISO_pin, int SCK_pin, int CS_pin0, int CS_pin1, int CS_pin2, int CS_pin3)
{
	m_MOSI_io 	= MOSI_pin;
	m_MISO_io 	= MISO_pin;
	m_SCK_io    = SCK_pin;
	m_CS_io[0]  = CS_pin0;
	m_CS_io[1]  = CS_pin1;
	m_CS_io[2]  = CS_pin2;
	m_CS_io[3]  = CS_pin3;
	
	init();
}

