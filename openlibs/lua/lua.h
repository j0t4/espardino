#ifndef __ESPARDINO_LUA_H
#define __ESPARDINO_LUA_H

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

lua_State* LUA_init();
void LUA_console (lua_State *L);
void LUA_close   (lua_State *L);
void LUA_setup_fatfs(FATFS *fs);

#endif
