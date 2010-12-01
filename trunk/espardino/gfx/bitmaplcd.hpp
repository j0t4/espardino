#ifndef __BITMAPLCD_H
#define __BITMAPLCD_H
#include <micro214x.h>
#include <hasprintf.h>

#define FONT_8x16 1
#define FONT_6x8  2

#define FONT_INVERTED true
#define FONT_NORMAL   false

#define TRANSPARENT  1
#define INVERTED	 2

class BitmapLCD : public HasPrintf
{
	private:
		int m_x_size;
		int m_y_size;
		int m_x,m_y;
		bool m_inv;
		unsigned char *scr;

		int f_width,f_height;
		const unsigned char *f_data;

	public:
		BitmapLCD();
		void setSize(int x,int y) { m_x_size=x; m_y_size=y; }
	 	void line(int x0,int y0,int x1,int y1,int c);

	 	void gotoxy(int x,int y);
	 	void setFont(int t, bool inverted);
	     int write(char data); /* for printf compatibility */

	    int putImg(const unsigned char *img,int x, int y, int opts);
virtual void plot(int x, int y, int c)=0;
virtual void clear()=0;

};

#endif

