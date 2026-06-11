CC = gcc
CFLAGS = -I. -I../src -g
LIBS = $(shell pkg-config --cflags --libs gtk4 sdl2 SDL2_mixer) -lmpg123
SRCS = app.c ../new_file.c ../dirwork.c ../music_duration.c
TARGET = app

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $@ $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)