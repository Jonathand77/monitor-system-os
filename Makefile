CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`
SRC = src/main.c src/app.c src/gui.c src/monitor.c
EXEC = monitor-sistema-grafico

# Único punto de selección de plataforma: para soportar un nuevo SO basta
# con añadir su archivo src/platform_<so>.c (implementando platform.h) y
# una rama aquí; el resto del código no cambia.
ifeq ($(OS),Windows_NT)
    SRC += src/platform_windows.c
    LDFLAGS += -liphlpapi
    PLATFORM_SRC = src/platform_windows.c
    PLATFORM_LDFLAGS = -liphlpapi
else
    SRC += src/platform_linux.c
    PLATFORM_SRC = src/platform_linux.c
    PLATFORM_LDFLAGS =
endif

OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Tests de la lógica pura (sin dependencia de GTK).
TEST_SRC = tests/test_monitor.c src/monitor.c $(PLATFORM_SRC)
TEST_EXEC = test_runner

test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(TEST_SRC)
	$(CC) -Wall -g -Isrc -o $@ $(TEST_SRC) $(PLATFORM_LDFLAGS)

clean:
	rm -f $(OBJ) $(EXEC) $(TEST_EXEC)

.PHONY: all clean test
