CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`
SRC = src/main.c src/monitor.c src/gui.c
OBJ = $(SRC:.c=.o)
EXEC = monitor-sistema-grafico

all: $(EXEC)

$(EXEC): $(OBJ)
    $(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f $(OBJ) $(EXEC)

.PHONY: all clean