CC = gcc
CFLAGS = -I. -I../src -g
LIBS = $(shell pkg-config --cflags --libs gtk4 sdl2 SDL2_mixer) -lmpg123
SRCS = src/app.c src/audio.c src/dirwork.c src/music_duration.c
TARGET = build/app

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $@ $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)