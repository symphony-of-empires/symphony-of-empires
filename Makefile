CXX=g++
CXXFLAGS=-Wall -Wextra -Wshadow -std=c++17 -O2 -fno-exceptions -fno-rtti -g -Isrc -Isrc/client
ifdef WINDOWS
LIBS=-lopengl32 -lglu32 -llua -lintl
else
LIBS=-lGL -lGLU -llua5.4
endif
LIBS:=$(LIBS) -lpng -lSDL2 -lSDL2_ttf -lm -pthread

OBJS=$(shell find . -type f -iname '*.cpp')
OBJS:=$(patsubst ./src/%.cpp,./obj/%.o,$(OBJS))

ifdef WINDOWS
CXXFLAGS:=$(CXXFLAGS) -DWINDOWS
endif

ifdef UNIT_TEST
CXXFLAGS:=$(CXXFLAGS) -DUNIT_TEST
endif

build: dirs bin/main

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
