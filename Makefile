CC=gcc
CFLAGS=-Wall -Wextra -O3 -mtune=native -g
LIBS=-static -lpng -lSDL2 -lGL -lGLU -llua5.2 -lSDL2_ttf -lm

OBJS=$(patsubst ./src/%.c,./obj/%.o,$(shell find . -type f -iname '*.c'))

build: dirs bin/main
	cp -r data bin/

clean:
	rm -r bin obj

dirs:
	mkdir -p bin obj

bin/main: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) $< -c -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $< -c -o $@

.PHONY: dirs build clean
