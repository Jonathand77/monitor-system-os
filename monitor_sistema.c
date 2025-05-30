// monitor_sistema.c
// Monitor de sistema básico en C para Linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <time.h>

#define BUFFER_SIZE 1024

// Estructura para almacenar info de CPU
typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuStats;

CpuStats get_cpu_stats() {
    FILE *fp = fopen("/proc/stat", "r");
    CpuStats stats = {0};
    if (fp) {
        fscanf(fp, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
               &stats.user, &stats.nice, &stats.system, &stats.idle,
               &stats.iowait, &stats.irq, &stats.softirq, &stats.steal);
        fclose(fp);
    }
    return stats;
}

double calculate_cpu_usage(CpuStats prev, CpuStats curr) {
    unsigned long long prev_idle = prev.idle + prev.iowait;
    unsigned long long curr_idle = curr.idle + curr.iowait;

    unsigned long long prev_total = prev.user + prev.nice + prev.system + prev_idle +
                                    prev.irq + prev.softirq + prev.steal;
    unsigned long long curr_total = curr.user + curr.nice + curr.system + curr_idle +
                                    curr.irq + curr.softirq + curr.steal;

    double total_diff = (double)(curr_total - prev_total);
    double idle_diff = (double)(curr_idle - prev_idle);

    return (100.0 * (total_diff - idle_diff)) / total_diff;
}

void mostrar_memoria() {
    FILE *fp = fopen("/proc/meminfo", "r");
    char label[32];
    unsigned long mem_total = 0, mem_available = 0, value;
    char unit[16];

    if (fp) {
        while (fscanf(fp, "%31s %lu %15s", label, &value, unit) == 3) {
            if (strcmp(label, "MemTotal:") == 0) mem_total = value;
            else if (strcmp(label, "MemAvailable:") == 0) mem_available = value;
            if (mem_total && mem_available) break;
        }
        fclose(fp);
        printf("RAM: %lu MB / %lu MB disponibles\n", (mem_total - mem_available) / 1024, mem_total / 1024);
    }
}

void mostrar_procesos() {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    char path[BUFFER_SIZE], line[BUFFER_SIZE];

    printf("\n%-8s %-25s\n", "PID", "Nombre del Proceso");
    printf("----------------------------------------\n");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
            FILE *fp = fopen(path, "r");
            if (fp) {
                char nombre[BUFFER_SIZE] = "";
                while (fgets(line, sizeof(line), fp)) {
                    if (strncmp(line, "Name:", 5) == 0) {
                        sscanf(line, "Name:\t%[^\n]", nombre);
                        break;
                    }
                }
                fclose(fp);
                printf("%-8s %-25s\n", entry->d_name, nombre);
            }
        }
    }
    closedir(dir);
}

void mostrar_disco() {
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        unsigned long total = (stat.f_blocks * stat.f_frsize) / (1024 * 1024);
        unsigned long libre = (stat.f_bfree * stat.f_frsize) / (1024 * 1024);
        printf("\nDisco: %lu MB libres de %lu MB totales\n", libre, total);
    }
}

void mostrar_red(unsigned long long *prev_rx, unsigned long long *prev_tx) {
    FILE *fp = fopen("/proc/net/dev", "r");
    char line[BUFFER_SIZE];
    unsigned long long total_rx = 0, total_tx = 0;

    if (!fp) return;
    fgets(line, sizeof(line), fp); // Skip header
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char iface[32];
        unsigned long long rx, tx;
        sscanf(line, "%31[^:]: %llu %*u %*u %*u %*u %*u %*u %*u %llu",
               iface, &rx, &tx);
        total_rx += rx;
        total_tx += tx;
    }
    fclose(fp);

    unsigned long long diff_rx = total_rx - *prev_rx;
    unsigned long long diff_tx = total_tx - *prev_tx;
    printf("\nRed: %.2f KB/s recibidos | %.2f KB/s enviados\n",
           diff_rx / 1024.0 / 3.0, diff_tx / 1024.0 / 3.0);

    *prev_rx = total_rx;
    *prev_tx = total_tx;
}

int main() {
    CpuStats prev_stats = get_cpu_stats();
    unsigned long long prev_rx = 0, prev_tx = 0;

    sleep(1);

    while (1) {
        system("clear");
        CpuStats curr_stats = get_cpu_stats();
        double cpu_usage = calculate_cpu_usage(prev_stats, curr_stats);
        prev_stats = curr_stats;

        printf("Uso de CPU: %.2f%%\n", cpu_usage);
        mostrar_memoria();
        mostrar_disco();
        mostrar_red(&prev_rx, &prev_tx);
        mostrar_procesos();

        printf("\n[Actualizando en 3 segundos...]\n");
        sleep(3);
    }
    return 0;
}
