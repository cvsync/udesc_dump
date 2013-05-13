#
# Makefile for udesc_dump
#
#	$Id: Makefile,v 1.10 2003/01/11 16:40:33 maekawa Exp $
#

PREFIX ?= /usr/local

PROG	= udesc_dump
SRCS	= main.c audio.c cs.c hid.c hid_items.c standard.c

CFLAGS += -I${.CURDIR}/include -g
CFLAGS += -W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual \
	  -Wchar-subscripts -Werror -Winline -Wmissing-prototypes \
	  -Wnested-externs -Wpointer-arith -Wredundant-decls -Wshadow \
	  -Wstrict-prototypes -Wwrite-strings

MAN	= udesc_dump.8
MAN8	= udesc_dump.8

BINDIR ?= ${PREFIX}/bin
MANDIR ?= ${PREFIX}/man

.include <bsd.prog.mk>
