#ifndef __SSD1308_H
#define __SSD1308_H

#include <micro214x.h>

class SSD1308 : public BitmapLCD
{
	private:
		DigitalIO m_DC;
		DigitalIO m_RST;
		Spi m_spi;

		unsigned char m_contrast;
		unsigned char m_scr[128*64/8];

		void set_dc() { m_DC.on();  }
		void clr_dc() { m_DC.off(); }

		void write_command(unsigned char d);
		void write_data(unsigned char d);



	public:


		SSD1308();
		SSD1308(int MISO_pin,int MOSI_pin, int SCK_pin, int CS_pin, int DC_pin, int RST_pin);
		void attach(int MISO_pin,int MOSI_pin, int SCK_pin, int CS_pin, int DC_pin, int RST_pin);

		void init();
		void clear();

		void dumpInit();
		void dump();
		void setContrast(unsigned char contrast);
		unsigned char getContrast() { return m_contrast; }
		void plot(int p_y,int p_x,int c);
};

#endif
