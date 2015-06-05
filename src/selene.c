/* Selene : DirectFB framework using Lua
 *
 * 12/04/2014 LF : First version
 * 25/04/2014 LF : Use loadfile() for script
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "selene.h"
#include "directfb.h"
#include "MQTT.h"

#define VERSION 0.0001	/* major, minor, sub */

	/*
	 * Utility function
	 */
char *mystrdup(const char *as){	/* as strdup() is missing within C99, grrr ! */
	char *s;
	assert(as);
	assert(s = malloc(strlen(as)+1));
	strcpy(s, as);
	return s;
}


	/*
	 *  Lua stuffs
	 */
lua_State *L;

void clean_lua(void){
	lua_close(L);
}

void *luaL_checkuserdata(lua_State *L, int n){
	if(lua_isuserdata(L, n))
		return lua_touserdata(L, n);
	else {
		luaL_error(L, "parameter %d expected to be a userdata");
		return NULL;
	}
}

#ifdef DEBUG
void dumpstack(lua_State *L){
	int i;
	int top = lua_gettop(L);

	puts("*D* stack");
	for (i = 1; i <= top; i++){
		int t = lua_type(L, i);
		switch(t){
		case LUA_TSTRING:
			printf("`%s'", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			printf("%g", lua_tonumber(L, i));
			break;
		default:
			printf("%s", lua_typename(L, t));
			break;
		}
		printf("  ");
	}
	printf("\n");
}
#endif

	/*
	 * Main loop
	 */
static const struct luaL_reg seleneLib[] = {
#ifdef USE_DIRECTFB
	{"CooperativeConst", CooperativeConst},
	{"SetCooperativeLevel", SetCooperativeLevel},
	{"init", SetCooperativeLevel},	/* Alias for SetCooperativeLevel */
#endif
	{NULL, NULL}    /* End of definition */
};

int main (int ac, char **av){
	char l[1024];

	L = lua_open();		/* opens Lua */
	luaL_openlibs(L);	/* and it's libraries */
	atexit(clean_lua);
	luaL_openlib(L,"Selene", seleneLib, 0);	/* Declare Selene's functions */

#ifdef USE_DIRECTFB
	init_directfb(L, &ac, &av);
#endif
#ifdef USE_MQTT
	init_mqtt(L);
#endif
	lua_pushnumber(L, VERSION);		/* Expose version to lua side */
	lua_setglobal(L, "SELENE_VERSION");

	if(ac > 1){
		int err = luaL_loadfile(L, av[1]) || lua_pcall(L, 0, 0, 0);
		if(err){
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1);  /* pop error message from the stack */
			exit(EXIT_FAILURE);
		}
	} else while(fgets(l, sizeof(l), stdin) != NULL){
		int err = luaL_loadbuffer(L, l, strlen(l), "line") || lua_pcall(L, 0, 0, 0);
		if(err){
			fprintf(stderr, "%s", lua_tostring(L, -1));
			lua_pop(L, 1);  /* pop error message from the stack */
		}
	}

	exit(EXIT_SUCCESS);
}

