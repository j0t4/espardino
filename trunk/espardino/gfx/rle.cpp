#include <micro214x.h>

void RLE::init(void *src,int len)
{
	this->p = src;
	this->rep = 0;
	this->last_c = 0;
	this->state = 0;
}

unsigned char RLE::get()
{

	switch (this->state)
	{
		case 0:
			this->last_c = *(this->p++);
			this->state=1;
			return this->last_c;

		case 1:
			if (this->last_c == *(this->p)) this->state=2;
			this->last_c=*(this->p);
			return *(this->p++);

		case 2:						/* retrieve length, fall back to 3 */
			this->rep = *(this->p++);
			this->state=3;

		case 3:						/* give repeated chars, if empty, get a new one */
			if (this->rep==0)
			{
				this->last_c = *(this->p++);
				this->state=1;
				return this->last_c;
			}
			else
			{
				this->rep--;
				return this->last_c;
			}
		default:
			return 0;
	}

}
