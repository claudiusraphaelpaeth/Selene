/* dfb_surface.c
 *
 * This file contains all stuffs related to DirectFB's surface.
 *
 * 24/04/2015 LF : First version (from a split of original directfb.c)
 */
#include "directfb.h"

#include <assert.h>

static const struct ConstTranscode _Capability[] = {
	{ "NONE", DSCAPS_NONE },
	{ "PRIMARY", DSCAPS_PRIMARY },
	{ "SYSTEMONLY", DSCAPS_SYSTEMONLY },
	{ "VIDEOONLY", DSCAPS_VIDEOONLY },
	{ "DOUBLE", DSCAPS_DOUBLE },
	{ "SUBSURFACE", DSCAPS_SUBSURFACE },
	{ "INTERLACED", DSCAPS_INTERLACED },
	{ "SEPARATED", DSCAPS_SEPARATED },
	{ "STATIC_ALLOC", DSCAPS_STATIC_ALLOC },
	{ "TRIPLE", DSCAPS_TRIPLE },
	{ "PREMULTIPLIED", DSCAPS_PREMULTIPLIED },
	{ "DEPTH", DSCAPS_DEPTH },
	{ "SHARED", DSCAPS_SHARED },
	{ "ROTATED", DSCAPS_ROTATED },
	{ "ALL", DSCAPS_ALL },
	{ "FLIPPING", DSCAPS_FLIPPING },
	{ NULL, 0 }
};

static int CapabilityConst( lua_State *L ){
	return findConst(L, _Capability);
}

static const struct ConstTranscode _TextLayout [] = {
	{ "LEFT", DSTF_LEFT },
	{ "CENTER", DSTF_CENTER },
	{ "RIGHT", DSTF_RIGHT },
	{ "TOP", DSTF_TOP },
	{ "BOTTOM", DSTF_BOTTOM },
	{ "OUTLINE", DSTF_OUTLINE },
	{ NULL, 0 }
};

static int TextLayoutConst( lua_State *L ){
	return findConst(L, _TextLayout);
}

static const struct ConstTranscode _DrawingFlags [] = {
	{"NOFX", DSDRAW_NOFX},
	{"BLEND", DSDRAW_BLEND},
	{"DST_COLORKEY", DSDRAW_DST_COLORKEY},
	{"SRC_PREMULTIPLY", DSDRAW_SRC_PREMULTIPLY},
	{"DST_PREMULTIPLY", DSDRAW_DST_PREMULTIPLY},
	{"DEMULTIPLY", DSDRAW_DEMULTIPLY},
	{"XOR", DSDRAW_XOR},
	{ NULL, 0 }
};

static int DrawingFlagsConst( lua_State *L ){
	return findConst(L, _DrawingFlags);
}

static int createsurface(lua_State *L){
	DFBResult err;
	IDirectFBSurface **sp;
	DFBSurfaceDescription dsc;
	assert(dfb);

	if(!lua_istable(L, -1)){	/* Argument has to be a table */
		lua_pushnil(L);
		lua_pushstring(L, "SelSurface.create() is expecting a table");
		return 2;
	}

	dsc.flags = 0;
	lua_pushstring(L, "caps");
	lua_gettable(L, -2);	/* Retrieve caps parameter if it exists */
	if(lua_type(L, -1) == LUA_TNUMBER){
		dsc.flags = DSDESC_CAPS;
		dsc.caps = luaL_checkint(L, -1);
	} else
		lua_pop(L, -1);	/* Remove the result we don't need */
/* tbd : other fields */

	sp = (IDirectFBSurface **)lua_newuserdata(L, sizeof(IDirectFBSurface *));
	luaL_getmetatable(L, "SelSurface");
	lua_setmetatable(L, -2);

	if((err = dfb->CreateSurface(dfb, &dsc, sp)) != DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}

	return 1;
}

static IDirectFBSurface **checkSelSurface(lua_State *L){
	void *r = luaL_checkudata(L, 1, "SelSurface");
	luaL_argcheck(L, r != NULL, 1, "'SelSurface' expected");
	return (IDirectFBSurface **)r;
}

static int SurfaceRelease(lua_State *L){
	IDirectFBSurface **s = checkSelSurface(L);

	if(!*s){
		lua_pushnil(L);
		lua_pushstring(L, "Release() on a dead object");
		return 2;
	}

	(*s)->Release(*s);
	*s = NULL;	/* Prevent double desallocation */

	return 0;
}

static int SurfaceGetSize(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	int w,h;

	if(!s){
		lua_pushnil(L);
		lua_pushstring(L, "GetSize() on a dead object");
		return 2;
	}

	if((err = s->GetSize(s, &w, &h)) != DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}

	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 2;
}


static int SurfaceFillRectangle(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);
	int w = luaL_checkint(L, 4);
	int h = luaL_checkint(L, 5);

	if((err = s->FillRectangle( s, x,y,w,h )) !=  DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}

	return 0;
}

static int SurfaceClear(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	int r = luaL_checkint(L, 2);
	int g = luaL_checkint(L, 3);
	int b = luaL_checkint(L, 4);
	int a = luaL_checkint(L, 5);

	if((err = s->Clear( s, r,g,b,a )) !=  DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}

	return 0;
}

static int SurfaceSetColor(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	int r = luaL_checkint(L, 2);
	int g = luaL_checkint(L, 3);
	int b = luaL_checkint(L, 4);
	int a = luaL_checkint(L, 5);

	if((err = s->SetColor( s, r,g,b,a )) !=  DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}
	return 0;
}

static int SurfaceDrawLine(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	int sx = luaL_checkint(L, 2);
	int sy = luaL_checkint(L, 3);
	int dx = luaL_checkint(L, 4);
	int dy = luaL_checkint(L, 5);

	if((err = s->DrawLine( s, sx,sy,dx,dy )) !=  DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}
	return 0;
}

static int SurfaceSetFont(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	IDirectFBFont **font = luaL_checkudata(L, 2, "SelFont");

	if(!font){
		lua_pushnil(L);
		lua_pushstring(L, "SelFont expected");
		return 2;
	}

	if((err = s->SetFont(s, *font)) !=  DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}
	return 0;
}

static int SurfaceDrawString(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	const char *msg = luaL_checkstring(L, 2);	/* Message to display */
	int x = luaL_checkint(L, 3);
	int y = luaL_checkint(L, 4);
	int alignment;

	if(lua_isnoneornil(L, 5))
		alignment = DSTF_TOPLEFT;
	else
		alignment = luaL_checkint(L, 5);

	if((err = s->DrawString(s, msg,-1,x,y,alignment)) !=  DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}
	return 0;
}

static int SurfaceSetDrawingFlags(lua_State *L){
	DFBResult err;
	IDirectFBSurface *s = *checkSelSurface(L);
	int flg = luaL_checkint(L, 2);

	if((err = s->SetDrawingFlags( s, flg )) !=  DFB_OK){
		lua_pushnil(L);
		lua_pushstring(L, DirectFBErrorString(err));
		return 2;
	}
	return 0;
}


static const struct luaL_reg SelSurfaceLib [] = {
	{"CapabilityConst", CapabilityConst},
	{"TextLayoutConst", TextLayoutConst},
	{"DrawingFlagsConst", DrawingFlagsConst},
	{"create", createsurface},
	{NULL, NULL}
};

static const struct luaL_reg SelSurfaceM [] = {
	{"Release", SurfaceRelease},
	{"destroy", SurfaceRelease},	/* Alias */
	{"GetSize", SurfaceGetSize},
	{"Clear", SurfaceClear},
	{"SetColor", SurfaceSetColor},
	{"SetDrawingFlags", SurfaceSetDrawingFlags},
	{"FillRectangle", SurfaceFillRectangle},
	{"DrawLine", SurfaceDrawLine},
	{"DrawString", SurfaceDrawString},
	{"SetFont", SurfaceSetFont},
	{NULL, NULL}
};

void _include_SelSurface( lua_State *L ){
	luaL_newmetatable(L, "SelSurface");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);	/* metatable.__index = metatable */
	luaL_register(L, NULL, SelSurfaceM);
	luaL_register(L,"SelSurface", SelSurfaceLib);
}

