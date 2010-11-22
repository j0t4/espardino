#ifndef __BITMAPLCD_H
#define __BITMAPLCD_H
#include <micro214x.h>
#include <hasprintf.h>

class BitmapLCD : public HasPrintf
{
	private:
		int m_x_size;
		int m_y_size;
		int m_x,m_y;
		unsigned char *scr;
		
	public:
		void setSize(int x,int y) { m_x_size=x; m_y_size=y; }
		void line(int x0,int y0,int x1,int y1,int c);
	   int write(char data); /* for printf compatibility */
	  void gotoxy(int x,int y);
	  void putImg(unsigned char *img,int x, int y, int opts);
	  virtual plot(int x, int y, int c)=0;
	  virtual clear()=0;
};	

#endif

