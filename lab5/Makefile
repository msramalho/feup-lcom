# Makefile for lab5

COMPILER_TYPE= gnu

CC=gcc

PROG= lab5
SRCS= lab5.c test5.c timer.c vbe.c video_gr.c keyboard.c read_xpm.c sprite.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>