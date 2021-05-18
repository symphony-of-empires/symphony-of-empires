ifdef CLANG
CXX=clang++
else
CXX=g++
endif
CXXFLAGS=-Wall -Wextra -Wshadow -std=c++17 -O3 -fno-stack-protector -mfpmath=sse -mmmx -msse -msse2 -m3dnow -fno-rtti -fno-exceptions -fopt-info-vec -iquote src -iquote src/client -g
ifdef WINDOWS
LIBS=-lopengl32 -lglu32 -llua -lintl
else
LIBS=-lGL -lGLU -llua5.4
endif
LIBS:=$(LIBS) -lpng -lSDL2 -lSDL2_ttf -lm -pthread

ifdef DEBUG
CXXFLAGS:=${CXXFLAGS} -fsanitize=address
endif

SRCS=$(shell find . -type f -iname '*.cpp')
OBJS:=$(patsubst ./src/%.cpp,./obj/%.o,$(SRCS))

ifdef WINDOWS
CXXFLAGS:=$(CXXFLAGS) -DWIN32
endif

ifdef UNIT_TEST
CXXFLAGS:=$(CXXFLAGS) -DUNIT_TEST
endif

build: dirs bin/main data/locale/ko/LC_MESSAGES/main.mo

depend: .depend

clean_build: clean build

.depend: $(SRCS)
	rm -f "$@"
	$(CXX) $(CXXFLAGS) -MM $^ > "$@"

include .depend

clean:
	@rm -r bin obj

dirs:
	mkdir -p bin obj obj/client
	mkdir -p data/locale/ko/LC_MESSAGES
	mkdir -p data/locale/es/LC_MESSAGES

bin/main: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

obj/%.o: src/%.cpp src/%.hpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

data/locale/ko/%.po: data/locale/%.pot
	msgmerge --update $@ $<
data/locale/ko/LC_MESSAGES/%.mo: data/locale/ko/%.po
	msgfmt --output-file=$@ $<
data/locale/ko/LC_MESSAGES:
	mkdir -p $@

data/locale/es/%.po: data/locale/%.pot
	msgmerge --update $@ $<
data/locale/es/LC_MESSAGES/%.mo: data/locale/es/%.po
	msgfmt --output-file=$@ $<
data/locale/es/LC_MESSAGES:
	mkdir -p $@

.PHONY: dirs build clean
