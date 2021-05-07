CXX=g++
CXXFLAGS=-Wall -Wextra -Wshadow -Wdouble-promotion -std=c++11 -O0 -g
ifdef WINDOWS
LIBS=-lopengl32 -lglu32 -llua -lintl
else
LIBS=-lGL -lGLU -llua5.4
endif
LIBS:=$(LIBS) -lpng -lSDL2 -lSDL2_ttf -lm

OBJS=$(patsubst ./src/%.c,./obj/%.o,$(shell find . -type f -iname '*.cpp'))

ifdef WINDOWS
CXXFLAGS:=$(CXXFLAGS) -DHAS_WINDOWS
endif #WINDOWS

build: dirs bin/main

clean:
	rm -r bin obj

dirs:
	mkdir -p bin obj

bin/main: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

obj/%.o: src/%.cpp src/%.hpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: dirs build clean
