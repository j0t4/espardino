//
// Copyright (c) 2009  http://www.espardino.com 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <LPC214x.h>
#include <vcom.h>
#include <string.h>
#include <micro214x.h>
#include <ff.h>
#include <lua.h>


void exec_from_file(lua_State *L,char *filename);
void exec_from_string(lua_State *L);
void run_lua_console(void);
static int my_c_function (lua_State *L);
	

void run_lua_console(void)
{
	
	xprintf("\n");
	xprintf("http://www.espardino.com           micro2148 LUA Console\n");
	xprintf("--------------------------------------------------------\n");
	xprintf("\n");
	
	lua_State *L=LUA_init(); // basic baselib, io, and math
 	
	lua_register(L,"my_c_function",my_c_function);
	
	LUA_console(L);
	
	//exec_from_file(L,"main.lua"); // <-- other examples
	//exec_from_string(L);
		
	LUA_close(L);
	
}

int main(void)
{

	FATFS fs;
	
	LEDS_init();	/* init the LEDs for this board  */
	VCOM_init();	/* init the USB Virtual COM port */
	
	xprintf_output(&VCOM_putchar_nl);	/* define the output for the printf/puts */
	
    LEDS_on(LED3);
	
	f_mount(0,&fs); 		// initialize the SD card filesystem
	LUA_setup_fatfs(&fs); 	// tell LUA where his filesystem is 
	
	VCOM_getchar();
	
	run_lua_console();	
	
	
}

static int my_c_function (lua_State *L)
{
	double num;
	
	int n=lua_gettop(L);
	
	if (n!=1) luaL_error(L,"this function takes 1 argument");
	
	num = lua_tonumber(L,1); /* get the parameter */
	
	lua_pushnumber(L,num*2.0); /* set the return value in stack */
	
	return 1; /* we return 1 value */
}



void exec_from_string(lua_State *L)
{

	if (luaL_dostring(L,"function myfun(x)\n"
						" if x==10 then x=\"aaa\" end\n"
						" print(x)\n"
						"end\n"
						"\n"
						"for i=1,20 do\n"
						" myfun(math.sin(i))\n" //
						"end\n"
													
						)!=0) 						
		{
			xputs("ERROR:");
			xputs(lua_tostring(L,-1));
			xputs("\n");
		}

}

void exec_from_file(lua_State *L,char *filename)
{
	
		if (luaL_loadfile(L,filename)!=0)
		{
			
			xputs("ERROR:");
			xputs(lua_tostring(L,-1));
			xputs("\n");
		}
		else
		{
			xprintf("executing %s>>\n",filename);
			if (lua_pcall(L,0,0,0)!=0)
			{
				xputs("ERROR:");
				xputs(lua_tostring(L,-1));
				xputs("\n");
			}
		}

}


