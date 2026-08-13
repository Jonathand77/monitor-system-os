#include "monitor.h"
#include "platform.h"

CpuStats get_cpu_stats(void) {
    return platform_get_ops()->get_cpu_stats();
}

// Fórmula única para ambas plataformas: en Windows los campos nice/iowait/
// irq/softirq/steal quedan en 0 (no se exponen vía GetSystemTimes), por lo
// que la misma fórmula "genérica" que usa Linux da el resultado correcto
// en los dos casos sin necesitar una rama #ifdef aquí.
double calculate_cpu_usage(CpuStats prev, CpuStats curr) {
    unsigned long long prev_idle = prev.idle + prev.iowait;
    unsigned long long curr_idle = curr.idle + curr.iowait;

    unsigned long long prev_non_idle = prev.user + prev.nice + prev.system +
                                        prev.irq + prev.softirq + prev.steal;
    unsigned long long curr_non_idle = curr.user + curr.nice + curr.system +
                                        curr.irq + curr.softirq + curr.steal;

    unsigned long long prev_total = prev_idle + prev_non_idle;
    unsigned long long curr_total = curr_idle + curr_non_idle;

    double totald = (double)(curr_total - prev_total);
    double idled = (double)(curr_idle - prev_idle);

    if (totald <= 0.0) return 0.0;
    return 100.0 * (totald - idled) / totald;
}

void monitor_get_memory_info(char *buffer, size_t size) {
    platform_get_ops()->get_memory_info(buffer, size);
}

void monitor_get_disk_info(char *buffer, size_t size) {
    platform_get_ops()->get_disk_info(buffer, size);
}

void monitor_get_network_info(unsigned long long *prev_rx, unsigned long long *prev_tx, char *buffer, size_t size) {
    platform_get_ops()->get_network_info(prev_rx, prev_tx, buffer, size);
}

void monitor_get_process_list(char *buffer, size_t size) {
    platform_get_ops()->get_process_list(buffer, size);
}
