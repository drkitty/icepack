#pragma once


#include "common.h"

#include <stdint.h>

#include "lua.h"


#define HASH_LEN 32


struct ipak {
    uint8_t hash[HASH_LEN];
    lua_State* L;
    struct ipak* parent;
};


void ipak_load(struct ipak* p, const char* name);
