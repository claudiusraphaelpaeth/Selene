/* csr_window.c
 *
 * This file contains all stuffs related to Curses' windows.
 *
 * 06/09/2016 LF : First version
 */
#include "curses.h"

#ifdef USE_CURSES

static WINDOW **checkSelCWindow(lua_State *L){
	void *r = luaL_checkudata(L, 1, "SelCWindow");
	luaL_argcheck(L, r != NULL, 1, "'SelCWindow' expected");
	return (WINDOW **)r;
}

static int CsRPrint( lua_State *L ){
	WINDOW **w = checkSelCWindow(L);

	char *arg = luaL_checkstring(L, 2);
	wprintw(*w, arg);

	return 0;
}

static int CsRRefresh( lua_State *L ){
	WINDOW **w = checkSelCWindow(L);

	wrefresh( *w );
	return 0;
}

static int SCW_delwin(lua_State *L){
	WINDOW **s = checkSelCWindow(L);

	if(!*s){
		lua_pushnil(L);
		lua_pushstring(L, "delwin() on a dead object");
		return 2;
	}

	if(*s != stdscr)
		delwin(*s);
	*s = NULL;

	return 0;
}

static const struct luaL_reg SelCWndLib [] = {
	{NULL, NULL}
};

static const struct luaL_reg SelCWndM [] = {
	{"print", CsRPrint},
	{"wprintw", CsRPrint},
	{"refresh", CsRRefresh},
	{"delwin", SCW_delwin},
	{"destroy", SCW_delwin},	/* Alias */
	{NULL, NULL}
};

void _include_SelCWindow( lua_State *L ){
	luaL_newmetatable(L, "SelCWindow");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);	/* metatable.__index = metatable */
	luaL_register(L, NULL, SelCWndM);
	luaL_register(L,"SelCWindow", SelCWndLib);
}

#endif