#include "common.h"

#include <stdbool.h>

#include "fail.h"
#include "forkutil.h"
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


static int GitRepo_get(lua_State* L)
{
    if (lua_gettop(L) != 1)
        fatal(E_RARE, "BUG: GitRepo.get takes one argument, 'self'");

    if (lua_getfield(L, -1, "remote") != LUA_TSTRING)
        fatal(E_COMMON, "'remote' must be a string");
    const char* remote = lua_tostring(L, -1);
    lua_pop(L, 1);

    if (lua_getfield(L, -1, "branch") != LUA_TSTRING)
        fatal(E_COMMON, "'branch' must be a string");
    const char* branch = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_pop(L, 1);

    v0("Using Git to clone remote '%s' branch '%s'", remote, branch);

    const char* repodir = "asdfqwerzxcv";

    fork_prog(
        &child_pid, "git", "git", "clone", "--depth", "1", "--branch", branch,
        remote, repodir, NULL
    );
    {
        int status;
        wait_prog(&child_pid, &status);
        if (status != 0)
            fatal(E_COMMON, "Can't clone Git repo (status %d)", status);
    }

    if (chdir(repodir) != 0)
        fatal_e(E_RARE, "Can't chdir into '%s'", repodir);

    return 0;
}


static void require_igloo(lua_State* L)
{
    int top = lua_gettop(L);

    int ret = luaL_dofile(L, "igloolib.lua");
    if (ret != LUA_OK)
        fatal(E_RARE, "BUG: %s", lua_tostring(L, -1));
    if (lua_gettop(L) != top)
        fatal(E_RARE, "BUG: igloolib.lua must not return a value");

    if (lua_getglobal(L, "GitRepo") != LUA_TTABLE)
        fatal(E_RARE, "BUG: 'GitRepo' must be a table");
    lua_pushcfunction(L, GitRepo_get);
    lua_setfield(L, -2, "get");
    lua_setglobal(L, "GitRepo");

    lua_settop(L, top);
}


static void ipak_set_hash(struct ipak* p, const char* name)
{
    unsigned int i;
    for (i = 0; i < HASH_LEN && name[i] != '\0'; ++i)
        p->hash[i] = name[i];
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
    int ret, count;

    p->L = luaL_newstate();
    require_standard(p->L);
    require_igloo(p->L);

    ret = luaL_dofile(p->L, name);
    if (ret != LUA_OK)
        fatal(E_COMMON, lua_tostring(p->L, -1));
    count = lua_gettop(p->L);
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
