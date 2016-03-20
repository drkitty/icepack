SUBMAKEFLAGS := $(MAKEFLAGS)
MAKEFLAGS += --no-builtin-rules
.SECONDEXPANSION:
.DELETE_ON_ERROR:

.DEFAULT_GOAL = all
.PHONY: all clean


EXE_SRC = icepack.c
SRC = $(EXE_SRC) fail.c

OBJ = $(SRC:%.c=%.o)
EXE = $(EXE_SRC:%.c=%)

CC = gcc
CFLAGS = -std=c99 -pedantic -g -Wall -Wextra -Werror \
	-Wno-unused-function -Wno-unused-parameter -I$(LUASRC)

LUASRC = lib/lua-5.3.2/src/


all: $(EXE) $(EXTRA_EXE)

$(OBJ): $$(patsubst %.o,%.c,$$@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE) $(EXTRA_EXE):
	$(CC) -o $@ $^

$(EXE): $$@.o

clean:
	rm -f $(OBJ) $(EXE)
	$(MAKE) -C $(LUASRC) clean


fail.o: common.h fail.h
icepack.o: common.h fail.h $(LUASRC)lauxlib.h $(LUASRC)lua.h $(LUASRC)lualib.h

icepack: fail.o icepack.o $(LUASRC)liblua.a


$(LUASRC)liblua.a:
	$(MAKE) -C $(LUASRC) linux MAKEFLAGS=$(SUBMAKEFLAGS)
