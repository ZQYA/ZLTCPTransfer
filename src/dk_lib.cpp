#include "dk_lib.hpp"
#include "lua_src/src/lua.h"
#include "lua_src/src/lauxlib.h"
#include "lua_src/src/lualib.h"
#include "dktool.hpp"
static int dl_start(lua_State *L) {
	bool stack_ok =	lua_checkstack(L,4);
	if(!stack_ok) {
	}	
	return 1;
}

static int dl_stop(lua_State *L) {
	return 1;
}
static const struct luaL_reg dklib [] = {
	{"dl_start",dl_start},
	{"dl_stop",dl_stop},
	{NULL,NULL}	
};
static int luaopen_dklib(lua_State *L) {
	luaL_openlib(L,"dklib",dklib,0);
	return 1;
}
