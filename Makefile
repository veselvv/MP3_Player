CC = gcc
CFLAGS = -I. -I./libs -g
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_mixer)
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer)
LDFLAGS = -lm
TARGETS  = src/new_file.c src/dirwork.c
BUILD = build/test_audio

all:
	$(CC) -o $(BUILD) $(TARGETS) $(CFLAGS) $(SDL_CFLAGS) $(SDL_LIBS) $(LDFLAGS)

clean:
	rm -f test_audio