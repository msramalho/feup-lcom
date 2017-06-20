# Makefile for the final project

COMPILER_TYPE= gnu

CC=gcc

PROG= proj
SRCS= proj.c main.c utils.c keyboard.c io.c graphics.c timer.c mouse.c read_xpm.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>