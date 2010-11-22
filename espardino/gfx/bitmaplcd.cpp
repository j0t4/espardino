#include <micro214x.h>

#define MYABS(x) (((x)<0)?(-(x)):(x))
#define SWAP(a,b) swptmp=a;a=b;b=swptmp;


// Bresenham integer optimized line drawing algorithm
// http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

void BitmapLCD::line(int x0,int y0,int x1,int y1,int c)
{
	int swptmp;
	int steep,deltax,deltay,error;
	int ystep,y,x;

	steep  = MYABS(y1-ºy0) > MYABS(x1-x0);

	if (steep)
	{
		SWAP(x0,y0);
		SWAP(x1,y1);
	}
	if (x0>x1)
	{
		SWAP(x0,x1);
		SWAP(y0,y1);
	}

	deltax = x1-x0;
	deltay = MYABS(y1-y0);
	error = deltax/2;

	y = y0;

	if (y0<y1)	ystep = 1;
	else		ystep = -1;

	for (x=x0;x<=x1;x++)
	{
		if (steep) plot(y,x,c);
		else       plot(x,y,c);
		error -= deltay;

		if (error<0)
		{
			y = y +ystep;
			error +=deltax;
		}
	}
}




void putImg(const char* img, short int x, short int y, int opts)
{
	unsigned char ix,iy;
	unsigned char sx,sy;
	unsigned char dx,dy;
	unsigned char rem_bits;
	unsigned char t_bits,bitval;

	if (img[0]!='E') return -1;  // not espardino img format
	if (img[1]!=1)   return -4;  // only version 1 supported
	if (img[2]!=1)   return -2;  // this driver only supports '1' bit image files

	sx = img[3];   // extract image X  - only supported 256 len -
	sy = img[5];   // extract image Y  - only supported 256 len -

	if (img[7]==1)
	{

		img+=8;  // update pointer to IMG data

		rem_bits = 0;

		t_bits = 0;

	    dy=y;

		for (iy=0;iy<sy;iy++)
		{
			dx=x;
			for (ix=0;ix<sx;ix++)
			{
				if (rem_bits<=0)
				{
					rem_bits=(*img)&0x7F;
					t_bits = ((*img)>>7);
					img++;
				}
				rem_bits--;
				if ((opts&TRANSPARENT)&&(!t_bits)){ dx++; continue;}
				if (opts&INVERTED) t_bits=!t_bits;
				plot(dx,dy,t_bits);
				dx++;
			}
			dy++;
		}
	} /* compressed format 1 */
	else if (img[7]==2)
	{
		
		RLE rle(img+8);
		rem_bits = 0;
		t_bits = 0;
		dy = y;
		for (iy=0;iy<sy;iy++)
		{
			dx = x;
			for (ix=0;ix<sx;ix++)
			{
				
				if (rem_bits<=0)
				{
					rem_bits = 8;
					t_bits = rle.get();
				}

				if (t_bits&0x80) bitval=1;
				else			 bitval=0;

				t_bits = t_bits << 1;
				rem_bits--;

				if ((opts&TRANSPARENT)&&(!bitval)){ dx++; continue; }

				if (opts&INVERTED) bitval=!bitval;
				plot(scr,dx,dy,bitval); /* bit={1->white 0->blue} */
				dx++;
			}
			dy++;
		}
	}

	return 0;
}



void BitmapLCD::gotoxy (int x, int y)
{
	m_x = x;
	m_y = y;
}

/*
void lcd_NewLine(unsigned char fontHeight, unsigned char offset);
void lcd_NewLine(unsigned char fontHeight, unsigned char offset) 
{
	if (m_y + fontHeight < 64)
		lcd_gotoxy(offset, m_y + fontHeight);
	else
		lcd_gotoxy(offset, 0);
}

*/

int write(char c)
{
	unsigned int index;
	unsigned char pages, tmp, i, j, xPos, yPos, xtmp,ibit;
	unsigned char last;
	
	if (c=='\f')
	{
		clear();
		return;
	}		
	
	if(c == '\n')
	{
		if (m_y+f->height<m_y_size) gotoxy(m_x,m_y + f->height);
		else								  		  gotoxy(m_x,m_y - m_y_size + f->height); 
		return;
	}
	
	if(c < 32)										// ignore escape characters
		return;
	
	xPos = m_x;									// save old coordinates
	yPos = m_y;
	
	c -= 32;
	
	pages = f->height/8;		// calculate pages
													// Small Font = 0;
													// BIG FONT	  = 1 ->upper;
	if(f->height%8 != 0)
		pages++;
	
	index = (unsigned int)(c) * f->width * pages;					// get the needed array index

	for(j = 0; j < pages; j++) 
	{
		xtmp = xPos;
		
		last = (f->width) * pages;
		for(i=j; i<= last; i += pages) 
		{
			if (i==last) tmp = 0x00;
			else 		 tmp = f->data[index + i];

			if (m_inv)							// NORMAL = 0, INV = 1;
				  tmp =~ tmp;						// Convert Data
			for (ibit=8;ibit;ibit--)
			{
				plot(m_x,m_y++,tmp&0x01);
				tmp >>=1;
			}
			m_y-=8; /* reverse all the pos we increased */

			gotoxy (xtmp++,m_y);

		}
		gotoxy(xPos, m_y + 8);
	}
	gotoxy(m_x + f->width, yPos);			// go to the upper right corner	
	
}

void BitmapLCD::setFont(tFont t)
{

	switch(t)
	{
		
		case FONT_8x16:	
				f.width 	= FONT8X16_WIDTH;
				f.height 	= FONT8X16_HEIGHT;
				f.data 		= Font8x16;
				break;
				
		case FONT_6x8:	
				f.width 	= FONT6X8_WIDTH;
				f.height 	= FONT6X8_HEIGHT;
				f.data	 	= Font6x8;
				break;
		eso estaba pensando yo
		
	}
			
}




void lcd_Printf (unsigned char *scr, unsigned char x, unsigned char y, char *string, unsigned char inv, unsigned char type)
{
		font_t f;
		m_inv 			= inv;
			
		if(!type)
		{
			f.width 	= FONT8X16_WIDTH;
			f.height 	= FONT8X16_HEIGHT;
			f.data 		= Font8x16;
		}
		else
		{
			f.width 	= FONT6X8_WIDTH;
			f.height 	= FONT6X8_HEIGHT;
			f.data	 	= Font6x8;
		}
			
		lcd_gotoxy (x,y);
		lcd_PutString (scr, string, &f);
	
}


void lcd_Printf_rom (unsigned char *scr, unsigned char x, unsigned char y, rom char *string, unsigned char inv, unsigned char type)
{
		font_t f;
		m_inv 			= inv;
			
		if(!type)
		{
			f.width 	= FONT8X16_WIDTH;
			f.height 	= FONT8X16_HEIGHT;
			f.data 		= Font8x16;
		}
		else
		{
			f.width 	= FONT6X8_WIDTH;
			f.height 	= FONT6X8_HEIGHT;
			f.data	 	= Font6x8;
		}
			
		lcd_gotoxy (x,y);
		lcd_PutString_rom (scr, string, &f);
	
}

