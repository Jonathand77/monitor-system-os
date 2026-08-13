#ifndef MONITOR_H
#define MONITOR_H

#include <stddef.h>

// Estructura para almacenar info de CPU
typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuStats;

// Fachada de monitoreo: expone los datos del sistema sin exponer
// cómo se obtienen en cada plataforma (ver platform.h).
CpuStats get_cpu_stats(void);
double calculate_cpu_usage(CpuStats prev, CpuStats curr);

void monitor_get_memory_info(char *buffer, size_t size);
void monitor_get_disk_info(char *buffer, size_t size);
void monitor_get_network_info(unsigned long long *prev_rx, unsigned long long *prev_tx, char *buffer, size_t size);
void monitor_get_process_list(char *buffer, size_t size);

#endif // MONITOR_H
