#ifndef __BUFFER_H
#define __BUFFER_H

#include <string.h>
#include <micro214x_types.h>

typedef struct {
			u8 *data;   //buffer data
			u32 first;				  //first data pointer
			u32 last;				  //last data pointer
			u32 saved_last;		  //last data pointer saved
			u32 overrun;			  //u8s which didn't fit
			u32 buffer_size;

			u32 ignore_until_next;
			u32 num;
} c_buffer;


typedef enum {res_OK,res_NODATA,res_FULL} tBufferResult;


void buffer_init        (c_buffer *buffer, u8 *data, u32 size);

void buffer_savelast    (c_buffer *buffer);
void buffer_restorelast (c_buffer *buffer);

tBufferResult buffer_put         (c_buffer *buffer, u8  data);
tBufferResult buffer_get         (c_buffer *buffer, u8 *data);
u32           buffer_num         (c_buffer *buffer);

void buffer_clear       (c_buffer *buffer);

#endif

