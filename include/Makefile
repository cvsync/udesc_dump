#
# The files hiddefs.h and hiddefs_data.h are generated from hiddefs.
#
#	$Id: Makefile,v 1.2 2002/12/03 16:10:21 maekawa Exp $
#

AWK=	awk
RM=	rm

# The targets are always remade.

.PHONY=	all

all:	hiddefs hiddefs2h.awk
	${RM} -f hiddefs.h hiddefs_data.h
	${AWK} -f hiddefs2h.awk hiddefs
