MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

.SECONDEXPANSION:
.DELETE_ON_ERROR:


EXE_SRC := tracer.c test.c
SRC := $(EXE_SRC) fail.c

OBJ := $(SRC:%.c=%.o)
EXE := $(EXE_SRC:%.c=%)

CC := gcc
CFLAGS := -std=c99 -pedantic -g -Wall -Wextra -Werror -Wno-unused-function \
	-DTRACER_AMD64


all: $(EXE) $(EXTRA_EXE)

$(OBJ): $$(patsubst %.o,%.c,$$@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE) $(EXTRA_EXE):
	$(CC) -o $@ $^

$(EXE): $$@.o

clean:
	rm -f $(OBJ) $(EXE)


fail.o: common.h fail.h
test.o: common.h
tracer.o: common.h fail.h

tracer: fail.o tracer.o
test: test.o


.DEFAULT_GOAL := all
.PHONY: all clean
