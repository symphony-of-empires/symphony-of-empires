CXX=g++
CXXFLAGS=-Wall -Wextra -O3 -mtune=native -g -fno-rtti -fno-exceptions
LIBS=-lpng -lSDL2 -lGL -lGLU -llua5.4 -lSDL2_ttf -lm

OBJS=$(patsubst ./src/%.c,./obj/%.o,$(shell find . -type f -iname '*.cpp'))

ifdef WINDOWS
CXXFLAGS:=-DHAS_WINDOWS
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
