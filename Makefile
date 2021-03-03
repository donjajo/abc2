CC = gcc
CFLAGS = -Wall
BUILDDIR = build/
SRC = $(wildcard shell/*.c) $(wildcard src/*.c)
BINS := $(SRC:%.c=%.o)
BUILDDIR = build/
SHELL = /bin/bash


.SUFFIXES:
.SUFFIXES: .c .o

abc2: ${BINS}
	${CC} ${CFLAGS} abc2.c $^ -o ${BUILDDIR}$@

%.o: %.c
	@mkdir -p ${BUILDDIR}
	@${CC} ${CFLAGS} -c $? -o $@