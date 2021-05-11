ifdef CLANG
CXX=clang++
else
CXX=g++
endif
CXXFLAGS=-Wall -Wextra -Wshadow -std=c++17 -O1 -iquote src -iquote src/client -g
ifdef WINDOWS
LIBS=-lopengl32 -lglu32 -llua -lintl
else
LIBS=-lGL -lGLU -llua5.4
endif
LIBS:=$(LIBS) -lpng -lSDL2 -lSDL2_ttf -lm -pthread

ifdef DEBUG
CXXFLAGS:=${CXXFLAGS} -fsanitize=address
endif

OBJS=$(shell find . -type f -iname '*.cpp')
OBJS:=$(patsubst ./src/%.cpp,./obj/%.o,$(OBJS))

ifdef WINDOWS
CXXFLAGS:=$(CXXFLAGS) -DWIN32
endif

ifdef UNIT_TEST
CXXFLAGS:=$(CXXFLAGS) -DUNIT_TEST
endif

build: dirs bin/main

clean_build: clean build

clean:
	rm -rf bin obj

dirs:
	mkdir -p bin obj obj/client

bin/main: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

obj/%.o: src/%.cpp src/%.hpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: dirs build clean
