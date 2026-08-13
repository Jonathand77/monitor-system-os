#include "app.h"
#include "monitor.h"

#include <stdio.h>

static CpuStats prev_cpu_stats = {0};
static unsigned long long prev_rx = 0, prev_tx = 0;

void app_refresh(AppSnapshot *snapshot) {
    CpuStats curr_cpu_stats = get_cpu_stats();
    double cpu_usage = calculate_cpu_usage(prev_cpu_stats, curr_cpu_stats);
    prev_cpu_stats = curr_cpu_stats;
    snprintf(snapshot->cpu, sizeof(snapshot->cpu), "Uso de CPU: %.2f%%", cpu_usage);

    monitor_get_memory_info(snapshot->memory, sizeof(snapshot->memory));
    monitor_get_disk_info(snapshot->disk, sizeof(snapshot->disk));
    monitor_get_network_info(&prev_rx, &prev_tx, snapshot->network, sizeof(snapshot->network));
    monitor_get_process_list(snapshot->processes, sizeof(snapshot->processes));
}
