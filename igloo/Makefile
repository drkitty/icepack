SUBMAKEFLAGS := $(MAKEFLAGS)
MAKEFLAGS += --no-builtin-rules
.SECONDEXPANSION:
.DELETE_ON_ERROR:

.DEFAULT_GOAL = all
.PHONY: all clean


EXE_SRC = icepack.c
SRC = $(EXE_SRC) fail.c igloo.c

OBJ = $(SRC:%.c=%.o)
EXE = $(EXE_SRC:%.c=%)

CC = gcc
CFLAGS = -std=c99 -pedantic -g -Wall -Wextra -Werror \
	-Wno-unused-function -Wno-unused-parameter -Wno-unused-variable \
	-Wno-unused-but-set-variable -I$(LUA_SRC)
LDFLAGS = -lm -ldl

LUA_TOP = lib/lua-5.3.2/
LUA_SRC = $(LUA_TOP)src/
LUA_HDRS = $(LUA_SRC)lauxlib.h $(LUA_SRC)lua.h $(LUA_SRC)lualib.h

all: $(EXE)

$(OBJ): $$(patsubst %.o,%.c,$$@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE):
	$(CC) -o $@ $^ $(LDFLAGS)

$(EXE): $$@.o

clean:
	rm -f $(OBJ) $(EXE)
	$(MAKE) -C $(LUA_SRC) clean


fail.o: common.h fail.h
icepack.o: common.h fail.h igloo.h
igloo.o: common.h fail.h igloo.h $(LUA_HDRS)

icepack: fail.o icepack.o igloo.o $(LUA_SRC)liblua.a


$(LUA_SRC)liblua.a:
	$(MAKE) -C $(LUA_TOP) linux MAKEFLAGS="$(SUBMAKEFLAGS)"
