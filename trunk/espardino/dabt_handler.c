/***************************************************************/
/* Copyright (c) 2009 NBEE Embedded Systems S.L.               */
/* All rights reserved.                                        */
/*                                                             */
/* Permission is hereby granted, free of charge, to any        */
/* person obtaining a copy of this software and associated     */
/* documentation files (the "Software"), to deal in the        */
/* Software without restriction, including without limitation  */
/* the rights to use, copy, modify, merge, publish,            */
/* distribute, sublicense, and/or sell copies of the           */
/* Software, and to permit persons to whom the Software is     */
/* furnished to do so, subject to the following conditions:    */
/*                                                             */
/* This code is distributed only with and for Espardino(tm)    */
/* products or derivative final products not intended as       */
/* development systems.                                        */
/*                                                             */
/* The above copyright notice and this permission notice       */
/* shall be included in all copies or substantial portions of  */
/* the Software.                                               */
/*                                                             */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY   */
/* KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE  */
/* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR     */
/* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS  */
/* OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR    */
/* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR  */
/* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE   */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.      */
/***************************************************************/

#include <LPC214x.h>
#include <micro214x.h>

typedef struct st_dabtdump
{
	unsigned int _PC;
	unsigned int _INSTR;
	unsigned int _CSPR;
	unsigned int _LR;
	unsigned int _SP;
	unsigned int _TYPE;
	unsigned int R[13]; // R0-12
} dabt_dump_data;

#define N_BIT (1<<31)
#define Z_BIT (1<<30)
#define C_BIT (1<<29)
#define V_BIT (1<<28)

#define TYPE_DABT 1
#define TYPE_PABT 2
#define TYPE_UND  3

#define IRQ_BIT (1<<7)
#define FIQ_BIT (1<<6)
#define THUMB_BIT (1<<5)
#define MODE_MASK 0x1f

const char *cspr_mode_to_str(unsigned int _CSPR);

const char *cspr_mode_to_str(unsigned int _CSPR)
{
	switch(_CSPR & MODE_MASK)
	{
	
		case 0x10: return "USR";
		case 0x1F: return "SYS";
		case 0x13: return "SVC";
		case 0x17: return "ABT";
		case 0x11: return "FIQ";
		case 0x12: return "IRQ";
		case 0x18: return "UND";
		default:   return "???";
	}
}

const char *except_str(unsigned int _TYPE);
const char *except_str(unsigned int _TYPE)
{
	switch(_TYPE)
	{
	  case 0x1: return "DATA ABORT EXCEPTION ";
	  case 0x2: return "PREFETCH ABORT EXCEPT";
	  case 0x3: return "UNDEFINED INSTRUCTION";
	  default : return "  UNKNOWN EXCEPTION  ";
	 }
}



void dabt_put_hex(unsigned int data,int nbls)
{
	unsigned int data_t;
		const char _hex[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

	while(nbls) 
	{
		
		data_t = (data>>((nbls-1)*4))&0xF;
		xputc(_hex[data_t]);
		nbls--;
	}

}

void dabt_dump(dabt_dump_data *dump);
void dabt_dump(dabt_dump_data *dump)
{
	int regs,n;
	unsigned int *sp;
	unsigned int ram_end;
	
	xputs("\r\n============== ");    xputs(except_str(dump->_TYPE));    xputs(" ===============\r\n\r\n");
	
	xprintf("MODE=%s",cspr_mode_to_str(dump->_CSPR));
	if (dump->_CSPR & IRQ_BIT) xputs(", IRQ DISABLED");
	if (dump->_CSPR & FIQ_BIT) xputs(", FIQ DISABLED");
	
	if (dump->_CSPR & THUMB_BIT) xputs(", THUMB");
	else						 xputs(", ARM");
	
	if (dump->_CSPR & N_BIT) xputs(" <N>");
	if (dump->_CSPR & Z_BIT) xputs(" <Z>");
	if (dump->_CSPR & C_BIT) xputs(" <C>");
	if (dump->_CSPR & V_BIT) xputs(" <V>");
	
	xputs("\r\n");
	
	xputs("PC="); 
	
	
	dabt_put_hex(dump->_PC,8);
	
	xputs(" [");dabt_put_hex(dump->_PC,8);	xputs("] => ");
	
	if (dump->_TYPE==TYPE_PABT) xputs("????????");
	else
	{
		if (dump->_CSPR & THUMB_BIT) dabt_put_hex(dump->_INSTR,4);
		else						 dabt_put_hex(dump->_INSTR,8);
	}
	
	xputs(" LR="); dabt_put_hex(dump->_LR,8); xputs("\r\n\r\n");
	
	n=0;

	for (regs=0;regs<=12;regs++)
	{
		xprintf("r%d",regs);
		if (regs<10) xputs(" ");
		xputc('=');
		dabt_put_hex(dump->R[regs],8);
		xputc(' ');
		n++;
		if(n>=4)
		{
			xputs("\r\n");
			n=0;
		}
	}
	
	ram_end = 0x40008000;
	
	
	xputs("\r\n\r\nSTACK DUMP: (SP=");
	dabt_put_hex(dump->_SP,8);
	xputs(")\r\n");
	
	sp = (unsigned int *) (dump->_SP & (~(0x3))); // align
	
	for (n=0;n<16;n++)
	{
		if ((n%4)==0)
		{
			xputs("\r\n");	dabt_put_hex((unsigned int)sp,8);	xputs("> ");
		}
		
		if (((unsigned int)sp)<ram_end)
		{
			dabt_put_hex(*sp++,8);
			xputc(' ');
		} 
		else 
		{
			break;
		}
	}
	
		
	xputs("\r\n\r\n====================================================\r\n");
	
	while(1);
}



