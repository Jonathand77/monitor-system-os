#ifndef MONITOR_H
#define MONITOR_H

// Estructura para almacenar info de CPU
typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuStats;

// Funciones para obtener estadísticas de CPU
CpuStats get_cpu_stats();
double calculate_cpu_usage(CpuStats prev, CpuStats curr);

// Funciones para mostrar información del sistema
void mostrar_memoria();
void mostrar_procesos();
void mostrar_disco();
void mostrar_red(unsigned long long *prev_rx, unsigned long long *prev_tx);

#endif // MONITOR_H