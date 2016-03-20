#pragma once


#include "common.h"

#include "lua.h"


struct igloo_state {
    lua_State* L;
};


struct igloo_state igloo_init();

void igloo_exec(struct igloo_state s, const char* name);
