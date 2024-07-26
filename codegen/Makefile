# flagovi za kompajliranje i linkanje
# -g je za debug
# -Wall pali sva upozorenja
# -Wno-unused da ne upozorava ako se ne koristi return value
# -Wno-write-strings miče upozorenje za konverziju string konst u char*
# -I dodaje putanju do headera


CFLAGS= -g -Wall -Wno-unused -Wno-deprecated -Wno-write-strings -Wno-nonnull-compare -Wno-register -DDEBUG -I.

.PHONY: codegen
.PHONY: codegen_test

codegen:	
# kompajlira pomocne datoteke bez linkanja
# -c samo kompajlira, ne linka
	g++ ${CFLAGS} -c cgen.cc
	g++ ${CFLAGS} -c cgen_supp.cc
	g++ ${CFLAGS} -c cgen-phase.cc
	g++ ${CFLAGS} -c utilities.cc
	g++ ${CFLAGS} -c stringtab.cc
	g++ ${CFLAGS} -c dumptype.cc
	g++ ${CFLAGS} -c tree.cc
	g++ ${CFLAGS} -c cool-tree.cc
	g++ ${CFLAGS} -c ast-lex.cc
	g++ ${CFLAGS} -c ast-parse.cc
	g++ ${CFLAGS} -c handle_flags.cc
# na kraju linka sve kompajlirane datoteke skupa
# -lfl je uključivanje biblioteke libfl.a od flexa
# -o je ime izlazne datoteke
	g++ ${CFLAGS} ast-lex.o ast-parse.o cgen.o cgen-phase.o cgen_supp.o cool-tree.o dumptype.o handle_flags.o stringtab.o tree.o utilities.o -lfl -o codegen
	
codegen_test:
	g++ codegen_test.cpp -o codegen_test
