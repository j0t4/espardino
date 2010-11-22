#include <micro214x.h>

#define SET_PAGE	0xB0

SSD1308::SSD1308()
{
}

SSD1308::SSD1308(int MISO_pin,int MOSI_pin, int SCK_pin, int CS_pin, int DC_pin, int RST_pin)
{

	attach(MISO_pin,MOSI_pin,SCK_pin,CS_pin,DC_pin,RST_pin);

}
void SSD1308::attach(int MISO_pin,int MOSI_pin, int SCK_pin, int CS_pin, int DC_pin, int RST_pin)
{
		m_DC.attach(DC_pin);
		m_RST.attach(RST_pin);
		m_spi.setBps(15000000);
		m_spi.attach(MISO_pin,MOSI_pin,SCK_pin,CS_pin);

}


void SSD1308::write_command(unsigned char d)
{
	clr_dc();
	m_spi.chipSelect(0);
	m_spi.xfer(d);
	m_spi.deSelect();
	set_dc();
}

void SSD1308::write_data(unsigned char d)
{
	set_dc();
	m_spi.chipSelect(0);
	m_spi.xfer(d);
	m_spi.deSelect();

}



void SSD1308::init()
{

	 delay_init();

	 m_RST.off();
   delay_ms(10);
   m_RST.on();

   write_command(0xae); // display off
   write_command(0xa1); //segment remap (column 127  is mapped to SEG0)
   write_command(0xda); //common pads hardware: alternative
   write_command(0x12);
   write_command(0xc8); //common output scan direction:com63~com0
   write_command(0xa8); //multiplex ration mode:63
   write_command(0x3f);
   write_command(0xd5); //display divide ratio/osc. freq. mode
   write_command(0x80);
   write_command(0x81); //contrast control
   write_command(0x50);
   write_command(0xd9); //set pre-charge period
   write_command(0x21);
   write_command(0x20); //Set Memory Addressing Mode
   write_command(0x00); // Page addressing mode
   write_command(0xdb); //VCOM deselect level mode
   write_command(0x30);
   write_command(0xad); //master configuration
   write_command(0x00);
   write_command(0xa4); //out follows RAM content
   write_command(0xa6); //set normal display

   clear();
	 dumpInit();
   dump();


   write_command(0xaf); // display on




}

void SSD1308::clear()
{
	unsigned int i;
	for (i=0;i<sizeof(m_scr);i++)
		m_scr[i]=0;

}
void SSD1308::dump()
{
	int x,y;
	unsigned char *p;

	set_dc();
	m_spi.chipSelect(0);
	for (x=0;x<8;x++)
	{

		p = &(m_scr[x]);
		for (y=0;y<128;y++)
		{
			m_spi.xfer(*p);
			p+=64/8;
		}

	}
	m_spi.deSelect();
}

void SSD1308::dumpInit(void)
{
		write_command(SET_PAGE /*+ x*/);	//set page
		write_command(0x00);
		write_command(0x10);
}


void SSD1308::setContrast(unsigned char contrast)
{
		write_command(0x81); //contrast control
    write_command(contrast);
    m_contrast = contrast;
}

void SSD1308::plot(unsigned char p_y,unsigned char p_x,int c)
{
	int addr;
	int bitmask;

	p_y = p_y & 0x7F;
	p_x = p_x & 0x3F;


	addr = (((int)p_y)<<3)+(p_x>>3);

	bitmask = 1<<(p_x&0x07);

	if (c)
	{
		m_scr[addr] |=bitmask;  /* set bit */
	}
	else
	{
		m_scr[addr] &= ~bitmask; /* clear bit */
	}
}





