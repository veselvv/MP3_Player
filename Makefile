CC = gcc
CFLAGS = -I. -I../src -g
LIBS = $(shell pkg-config --cflags --libs gtk4 sdl2 SDL2_mixer) -lmpg123
SRCS = src/app.c src/audio.c src/dirwork.c src/music_duration.c
TARGET = build/app

all: $(TARGET)

# Правило для создания папки build
build:
	mkdir -p build

# Сборка зависит от папки (порядокная зависимость)
$(TARGET): $(SRCS) | build
	$(CC) $(SRCS) -o $@ $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
	rmdir build 2>/dev/null || true