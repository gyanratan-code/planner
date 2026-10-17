# Compiler
CXX = g++
CXXFLAGS = -Wall -std=c++20 -MMD -MP
DEBUG ?= 0

# DEBUG Setting
ifeq (${DEBUG},1)
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -O2
endif

BUILD = build
LDLIB = -lsqlite3 -ledit

SOURCES = main.cpp $(wildcard app/*.cpp db/*.cpp task/*.cpp cli/*.cpp)
OBJECTS = $(patsubst %.cpp,${BUILD}/%.o,${SOURCES})

# Rules
plan : ${OBJECTS}
	${CXX} ${CXXFLAGS} -o plan $^ ${LDLIB}


${BUILD}/%.o : %.cpp
	mkdir -p ${dir $@}
	${CXX} ${CXXFLAGS} -c $< -o $@


-include ${OBJECTS:.o=.d}

# Cleaning up
clean :
	@rm -rf build plan test
	@echo "All cleaned."

.PHONY: clean