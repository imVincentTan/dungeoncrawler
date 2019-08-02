CXX = g++
CXXFLAGS = -g -std=c++14 -pedantic -Wall -Werror=vla -MMD
EXEC = cc3k+
TESTS = suite.txt
SRC = main.cc character.cc game.cc item.cc level.cc majoritem.cc npc.cc object.cc player.cc potion.cc textdisplay.cc treasure.cc randMapGen.cc
OBJ = ${SRC:.cc=.o}
DEP = ${OBJ:.o=.d}

${EXEC}: ${OBJ}
	${CXX} ${CXXFLAGS} ${OBJ} -o ${EXEC}

-include ${DEP}

.PHONY: clean release

clean:
	rm ${OBJ} ${DEP}

release: clean ${OBJ}
	${CXX} -O2 ${CXXFLAGS} ${OBJ} -o ${EXEC}
	../runSuite test/${TESTS} ./${EXEC}
