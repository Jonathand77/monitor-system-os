#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>
#include "monitor.h"

// Tabla de operaciones específicas de plataforma. Cada sistema operativo
// soportado provee una implementación completa en su propio archivo
// (platform_linux.c, platform_windows.c, ...) y la registra en
// platform_get_ops(). Para añadir una nueva plataforma basta con crear un
// nuevo archivo que implemente esta interfaz y enlazarlo en el Makefile:
// no hace falta modificar monitor.c, app.c, gui.c ni main.c.
typedef struct {
    CpuStats (*get_cpu_stats)(void);
    void (*get_memory_info)(char *buffer, size_t size);
    void (*get_disk_info)(char *buffer, size_t size);
    void (*get_network_info)(unsigned long long *prev_rx, unsigned long long *prev_tx, char *buffer, size_t size);
    void (*get_process_list)(char *buffer, size_t size);
} PlatformOps;

const PlatformOps *platform_get_ops(void);

#endif // PLATFORM_H
