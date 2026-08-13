#ifndef TEXT_BUFFER_H
#define TEXT_BUFFER_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Escribe una línea con snprintf en buffer[*used..size) de forma segura:
// - nunca deja que *used supere size (evita que "size - *used" haga
//   underflow en la siguiente llamada, que sería un desbordamiento).
// - si el contenido no cabe completo, corta ahí mismo y marca *truncated.
// Usado por los listados de procesos, que son de tamaño no acotado.
static inline void tb_append_line(char *buffer, size_t size, size_t *used, int *truncated, const char *fmt, ...) {
    if (*truncated || *used >= size) {
        *truncated = 1;
        return;
    }
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(buffer + *used, size - *used, fmt, args);
    va_end(args);

    if (written < 0) {
        *truncated = 1;
        return;
    }
    if ((size_t)written >= size - *used) {
        *used = size; // el contenido se cortó: no seguir escribiendo
        *truncated = 1;
        return;
    }
    *used += (size_t)written;
}

#endif // TEXT_BUFFER_H
