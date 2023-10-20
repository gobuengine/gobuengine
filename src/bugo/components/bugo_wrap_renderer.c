#include "bugo_wrap_renderer.h"
#include "bugo_renderer.h"
#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>

static int add_rotation()
{

}

void bugo_wrap_renderer_init(lua_State* L)
{
    lua_createtable(L, 0, 1);
        lua_pushstring(L, "add_rotation");
        // lua_pushlightuserdata(L, r->rotation);
        lua_pushcfunction(L, add_rotation);
    lua_settable(L, -3);
}

