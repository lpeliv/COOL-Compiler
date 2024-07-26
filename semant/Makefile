# flagovi za kompajliranje i linkanje
# -g je za debug
# -Wall pali sva upozorenja
# -Wno-unused da ne upozorava ako se ne koristi return value
# -Wno-write-strings miče upozorenje za konverziju string konst u char*
# -I dodaje putanju do headera


CFLAGS= -g -Wall -Wno-unused -Wno-deprecated -Wno-write-strings -Wno-register -Wno-nonnull-compare -DDEBUG -I.

.PHONY: semant
.PHONY: semant_test

semant:	
# kompajlira pomocne datoteke bez linkanja
# -c samo kompajlira, ne linka
	g++ ${CFLAGS} -c tree.cc
	g++ ${CFLAGS} -c cool-tree.cc
	g++ ${CFLAGS} -c dumptype.cc
	g++ ${CFLAGS} -c stringtab.cc
	g++ ${CFLAGS} -c utilities.cc
	g++ ${CFLAGS} -c ast-parse.cc
	g++ ${CFLAGS} -c ast-lex.cc
	g++ ${CFLAGS} -c handle_flags.cc
	g++ ${CFLAGS} -c semant-phase.cc
	g++ ${CFLAGS} -c semant.cc
# na kraju linka sve kompajlirane datoteke skupa
# -lfl je uključivanje biblioteke libfl.a od flexa
# -o je ime izlazne datoteke
	g++ ${CFLAGS} semant.o semant-phase.o handle_flags.o ast-lex.o ast-parse.o utilities.o stringtab.o dumptype.o tree.o cool-tree.o -lfl -o semant
	
semant_test:
	g++ semant_test.cpp -o semant_test

symtab_example:
# kompajlira pomocne datoteke bez linkanja
# -c samo kompajlira, ne linka
	g++ ${CFLAGS} -c symtab_example.cc
# na kraju linka sve kompajlirane datoteke skupa
# -lfl je uključivanje biblioteke libfl.a od flexa
# -o je ime izlazne datoteke
	g++ ${CFLAGS} symtab_example.o -lfl -o symtab_example