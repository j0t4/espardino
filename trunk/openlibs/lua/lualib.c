
/**************************** lualib.c **********************************/
/* NBEE Embedded Systems S.L.                                           */
/* All rights reserved.													*/
/*																		*/
/* Redistribution and use in source and binary forms, with or without	*/
/* modification, are permitted provided that the following conditions	*/
/* are met:																*/
/* 1. This code is distributed only with Espardino (tm) products, or    */
/*    derivative final products not intended as development systems.    */
/* 2. Redistributions of source code must retain the above copyright	*/
/*   notice, this list of conditions and the following disclaimer.		*/
/* 3. Redistributions in binary form must reproduce the above copyright	*/
/*   notice, this list of conditions and the following disclaimer in the*/
/*   documentation and/or other materials provided with the 			*/
/*   distribution.														*/
/* 4. The name of NBEE Embedded Systems S.L. or its contributors may    */
/* not be used to endorse or promote products derived from this software*/
/* without specific prior written permission.							*/
/*																		*/
/* THIS SOFTWARE IS PROVIDED BY NBEE Embedded Systems S.L "AS IS" AND 	*/
/* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE*/
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR	*/
/* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NBEE EMBEDDED SYSTEMS  BE	*/
/* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	*/
/* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	*/
/* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 		*/
/* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,*/
/* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE */
/* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 	*/
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.					*/
/************************************************************************/

#
#include <LPC214x.h>
#include <vcom.h>
#include <string.h>
#include <micro214x.h>
#include <stdio.h>
#include <stdlib.h>
#include "lua/src/lua.h"
#include "lua/src/lauxlib.h"
#include "lua/src/lualib.h"
#include "lua_io_wrappers.h"
#ifndef false
#define false 0
#define true 1 
#endif




/*

static int print(lua_State *L)
{
 int n=lua_gettop(L);
 int i;
 for (i=1; i<=n; i++)
 {
  if (i>1) xprintf("\t");
  if (lua_isstring(L,i))
   xprintf("%s",lua_tostring(L,i));
  else if (lua_isnil(L,i))
   xprintf("%s","nil");
  else if (lua_isboolean(L,i))
   xprintf("%s",lua_toboolean(L,i) ? "true" : "false");
  else
   xprintf("%s:%p",luaL_typename(L,i),lua_topointer(L,i));
 }
 xprintf("\n");
 return 0;
}

static int read_humidity_sensor(lua_State *L)
{
	
	lua_pushnumber(L,33+lua_tonumber(L,1));
	return 1;
}

*/

static const luaL_Reg lualibs[] = {
{"", luaopen_base}, // no coroutines 	//{LUA_LOADLIBNAME, luaopen_package},
										//{LUA_TABLIBNAME, luaopen_table},
{LUA_IOLIBNAME, luaopen_io},			//{LUA_OSLIBNAME, luaopen_os},
										//{LUA_STRLIBNAME, luaopen_string},
{LUA_MATHLIBNAME, luaopen_math},
										// {LUA_DBLIBNAME, luaopen_debug},
{NULL, NULL}
};


void openlibs (lua_State *L) {
  const luaL_Reg *lib = lualibs;
  for (; lib->func; lib++) {
    lua_pushcfunction(L, lib->func);
    lua_pushstring(L, lib->name);
    lua_call(L, 1, 0);
  }
}

static int foreach (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  luaL_checktype(L, 2, LUA_TFUNCTION);
  lua_pushnil(L);  /* first key */
  while (lua_next(L, 1)) {
    lua_pushvalue(L, 2);  /* function */
    lua_pushvalue(L, -3);  /* key */
    lua_pushvalue(L, -3);  /* value */
    lua_call(L, 2, 1);
    if (!lua_isnil(L, -1))
      return 1;
    lua_pop(L, 2);  /* remove value and result */
  }
  return 0;
}



void IO_check_port_pin(lua_State *L,int port,int pin);

void IO_check_port_pin(lua_State *L, int port,int pin)
{

	luaL_argcheck(L, (port>=0) && (port<=1), 1, "port must be 0 or 1");
	luaL_argcheck(L, (pin>=0) && (pin<=31), 2, "pin must be 0 .. 31");

}

static int IO_input_lua(lua_State *L)
{
	int port=lua_tonumber(L,1);
	int pin =lua_tonumber(L,2);
	
	IO_check_port_pin(L,port,pin);
	
	if (IO_input((port<<8)|pin))
	{
		lua_pushboolean(L,1);
	}
	else
	{
		lua_pushboolean(L,0);
	}
	
	return 1;
}

static int IO_output_low_lua(lua_State *L)
{
	int port=lua_tonumber(L,1);
	int pin =lua_tonumber(L,2);
	
	IO_check_port_pin(L,port,pin);
	
	IO_output_low((port<<8)|pin);
	
	return 0;
}


static int IO_output_high_lua(lua_State *L)
{
	int port=lua_tonumber(L,1);
	int pin =lua_tonumber(L,2);
	
	IO_check_port_pin(L,port,pin);
	
	IO_output_high((port<<8)|pin);
	
	return 0;
}


static int LEDS_on_lua(lua_State *L)
{
	int n=lua_gettop(L);
	int i,ledn;
	for (i=1; i<=n; i++)
	{
		ledn = lua_tonumber(L,i);
		switch(ledn)
		{
			case 1:	LEDS_on(LED1); break;
			case 2: LEDS_on(LED2); break;
			case 3: LEDS_on(LED3); break;
			default:
				luaL_argcheck(L, false, i, "led must be 1,2, or 3");
		}
	}
	
	return 0;
 
}

static int LEDS_off_lua(lua_State *L)
{
	int n=lua_gettop(L);
	int i,ledn;
	for (i=1; i<=n; i++)
	{
		ledn = lua_tonumber(L,i);
		switch(ledn)
		{
			case 1:	LEDS_off(LED1); break;
			case 2: LEDS_off(LED2); break;
			case 3: LEDS_off(LED3); break;
			default:
				luaL_argcheck(L, false, i, "led must be 1,2, or 3");
		}
	}
	
	return 0;

}


int ADC_check_port_pin(lua_State *L, int port,int pin,int adc_mask);

int ADC_check_port_pin(lua_State *L, int port,int pin,int adc_mask)
{

	luaL_argcheck(L, (port>=0) && (port<=1), 1, "port must be 0 or 1");
	luaL_argcheck(L, (pin>=0) && (pin<=31), 2, "pin must be 0 .. 31");
	if(!adc_mask)
		luaL_error(L,"this port/pin doesn't have analog input");

	if ((port>=0) && (port<=1) &&(pin>=0) && (pin<=31) && adc_mask)
	{
		return 1;
	}
	return 0;
}



static int ADC_input_lua(lua_State *L)
{
	int const ADC_masks[32] = 
			{0,0,0,0,AD0_6, AD0_7, AD1_0, 0, AD1_1, 0, AD1_2, 0, AD1_3, AD1_4, 0, AD1_5, 0,
			 0,0,0,0,AD1_6, AD1_7, 0, 0,AD0_4, 0,0,AD0_1, AD0_2, AD0_3,0};

	int port=lua_tonumber(L,1);
	int pin =lua_tonumber(L,2);
	int adc_mask=0;
	double fval;
	
	if ((port==0) && (pin>=0) && (pin<=31)) adc_mask = ADC_masks[pin];
	if (ADC_check_port_pin(L,port,pin,adc_mask))
	{
	
		ADC_init(adc_mask);
		fval = ADC_read(adc_mask);
		fval = fval / 1023.0f;
		
	}
	else
	{ 
		fval = -1;
	}
	
	lua_pushnumber(L,fval);
	return 1;
}



void delay_init()
{

    
	T0TC = 0;
	T0PR = (VPB_get_speed()/1000000)-1;
	T0TCR = 0x01;
	
}

void delay_us (unsigned long usdelay)
{
	
	unsigned int next_T0TC;
	next_T0TC = T0TC + usdelay; 
	while (T0TC<next_T0TC); /* we assume that we'll do it on time */
	
}

static int delay_ms_lua (lua_State *L)
{

	delay_us(lua_tonumber(L,1)*1000);
	return 0;
}

static int delay_us_lua (lua_State *L)
{

	delay_us(lua_tonumber(L,1));
	return 0;
}



void register_funcs(lua_State *L)
{
	delay_init();
	lua_register(L,"foreach",foreach);
	lua_register(L,"input",IO_input_lua);
	lua_register(L,"output_low",IO_output_low_lua);
	lua_register(L,"output_high",IO_output_high_lua);	
	lua_register(L,"leds_on",LEDS_on_lua);	
	lua_register(L,"leds_off",LEDS_off_lua);	
	lua_register(L,"adc_input",ADC_input_lua);	
	lua_register(L,"delay_us",delay_us_lua);	
	lua_register(L,"delay_ms",delay_ms_lua);	
}



LUALIB_API int luaopen_bit(lua_State *L);

lua_State *LUA_init()
{
	lua_State *L=lua_open();
	if (L)
	{
		register_funcs(L);
		openlibs(L);
		luaopen_bit(L);
		
	}
	return L;
}

void LUA_close(lua_State *L)
{
	lua_close(L);
}
