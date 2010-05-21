#include "buffer.h"

//////////////////////////////////////////////////////////
//  void buffer_init (c_buffer *buffer, u8 *data, u32 size)
//    	
//  setups the buffer pointer to data, and size
// 
void buffer_init (c_buffer *buffer, u8 *data, u32 size)
{
	memset(buffer,0,sizeof(c_buffer));
	buffer->buffer_size=size;
	buffer->data=data;
}


//////////////////////////////////////////////////////////
//  void buffer_put(c_buffer *buffer, u8 data)
//    	
//  puts a data u8 in the circular buffer 
// 
tBufferResult buffer_put(c_buffer *buffer, u8 data)
{
	if ((buffer_num(buffer)+1)>=buffer->buffer_size) 
	{
	 	buffer->overrun++;
		return res_FULL;
	}
	
	buffer->data[buffer->last]=data;

	buffer->last=buffer->last + 1;
	if (buffer->last>=buffer->buffer_size) 
		buffer->last=0;
       
	return res_OK;

}



//////////////////////////////////////////////////////////
//  int buffer_get(c_buffer *buffer,char *data)
//	
//	gets a u8 from a circular buffer,
// returns: 1 (u8 wrote to *data) / 0 (buffer empty)
//

tBufferResult buffer_get(c_buffer *buffer,u8 *data)
{
	if(buffer->first!=buffer->last)
	{
     	*data=buffer->data[buffer->first];

		buffer->first = buffer->first + 1;
		if (buffer->first>=buffer->buffer_size) buffer->first=0;

		return res_OK;
	}
	else return res_NODATA;
}


//////////////////////////////////////////////////////////
//  int buffer_num(c_buffer *buffer) {
//	
// returns: the number of u8s waiting to be readen in the
//          buffer.
//

u32 buffer_num(c_buffer *buffer) {

	u32 first,last;
    
	first=buffer->first;
	last =buffer->last;
	
	if (first>last) return ((last+buffer->buffer_size)-first);
	else			return ( last             		  -first);
    
}

//////////////////////////////////////////////////////////
//  void buffer_clear(c_buffer *buffer)
//
//   clears the buffer
//
void buffer_clear(c_buffer *buffer)
{
	buffer->first=0;
	buffer->last=0;
	buffer->overrun=0;
}
