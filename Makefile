CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lmingw32 -lSDLmain -lSDL -lSDL_ttf -lSDL_gfx -lSDL_mixer -lm

SRCDIR = src
INCDIR = include
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = projet

all: $(TARGET) run

run: $(TARGET)
	cd $(SRCDIR) && ../$(TARGET).exe

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET) $(TARGET).exe

.PHONY: all clean