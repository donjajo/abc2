CC		= gcc
PROG		= abc2
CFLAGS		= -Wall
BUILDDIR	= build/
SRC		= $(wildcard shell/*.c) $(wildcard src/*.c)
BINS		:= $(SRC:%.c=%.o)
BUILDDIR	= build/
SHELL		= /bin/bash

ifneq ($(DEBUG),)
	CFLAGS := $(CFLAGS) -g
endif

.PHONY: clean
all: $(PROG)

$(PROG): $(BINS)
	$(CC) $(CFLAGS) $(PROG).c $^ -o $(BUILDDIR)$@

%.o: %.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	git clean -Xfq
	rm -rf build/$(PROG)