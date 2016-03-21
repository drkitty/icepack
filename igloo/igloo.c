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


static void ipak_set_hash(struct ipak* p, const char* full_name)
{
    unsigned int i;
    for (i = 0; i < HASH_LEN && full_name[i] != '\0'; ++i)
        p->hash[i] = full_name[i];
    for (/* */; i < HASH_LEN; ++i)
        p->hash[i] = '\0';
    if (verbosity >= 2) {
        print("Hash is ");
        for (i = 0; i < HASH_LEN; ++i)
            printf("%02x", p->hash[i]);
        print("\n");
    }
}


void ipak_load(struct ipak* p, const char* name)
{
    p->L = luaL_newstate();
    require_standard(p->L);

    int idx = lua_gettop(p->L);
    int ret = luaL_dofile(p->L, name);
    if (ret != LUA_OK)
        fatal(E_COMMON, lua_tostring(p->L, -1));
    int count = lua_gettop(p->L) - idx;
    if (count != 1)
        fatal(E_COMMON, "'%s' returned %d values, not 1", name, count);
    if (lua_type(p->L, -1) != LUA_TTABLE)
        fatal(E_COMMON, "'%s' did not return a table");

    if (lua_getfield(p->L, -1, "name") != LUA_TSTRING)
        fatal(E_COMMON, "Invalid name");
    {
        const char* pkgname = lua_tostring(p->L, -1);
        v2("Package name is '%s'", pkgname);
        ipak_set_hash(p, pkgname);
    }
    lua_pop(p->L, 1);
}
