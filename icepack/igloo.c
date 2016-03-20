#include "common.h"

#include <stdbool.h>

#include "fail.h"
#include "igloo.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"


static void require_standard(lua_State* L)
{
    static const luaL_Reg libs[] = {
        {"_G", luaopen_base},
        {LUA_COLIBNAME, luaopen_coroutine},
        {LUA_TABLIBNAME, luaopen_table},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_MATHLIBNAME, luaopen_math},
        {LUA_UTF8LIBNAME, luaopen_utf8},
        {NULL, NULL},
    };

    for (const luaL_Reg* lib = libs; lib->name != NULL; ++lib) {
        luaL_requiref(L, lib->name, lib->func, true);
        lua_pop(L, 1);
    }
}


struct igloo_state igloo_init()
{
    struct igloo_state s;
    s.L = luaL_newstate();
    require_standard(s.L);
    return s;
}


void igloo_exec(struct igloo_state s, const char* name)
{
    int ret = luaL_dofile(s.L, name);
    if (ret != 0)
        fatal(E_COMMON, lua_tostring(s.L, -1));
}
