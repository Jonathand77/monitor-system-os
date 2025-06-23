CC = gcc
CFLAGS = -Wall -g
SRC = src/main.c src/monitor.c src/gui.c
OBJ = $(SRC:.c=.o)
EXEC = monitor-sistema-grafico

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean