#include "platform.h"
#include "text_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/statvfs.h>

static CpuStats linux_get_cpu_stats(void) {
    CpuStats stats = {0};
    FILE *fp = fopen("/proc/stat", "r");
    if (fp) {
        fscanf(fp, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
               &stats.user, &stats.nice, &stats.system, &stats.idle,
               &stats.iowait, &stats.irq, &stats.softirq, &stats.steal);
        fclose(fp);
    }
    return stats;
}

static void linux_get_memory_info(char *buffer, size_t size) {
    FILE *fp = fopen("/proc/meminfo", "r");
    unsigned long mem_total = 0, mem_available = 0, value;
    char label[32], unit[16];

    if (fp) {
        while (fscanf(fp, "%31s %lu %15s", label, &value, unit) == 3) {
            if (strcmp(label, "MemTotal:") == 0) mem_total = value;
            if (strcmp(label, "MemAvailable:") == 0) mem_available = value;
        }
        fclose(fp);
    }
    snprintf(buffer, size, "RAM: %lu MB disponibles / %lu MB totales", mem_available / 1024, mem_total / 1024);
}

static void linux_get_disk_info(char *buffer, size_t size) {
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        unsigned long total = (stat.f_blocks * stat.f_frsize) / (1024 * 1024);
        unsigned long free = (stat.f_bfree * stat.f_frsize) / (1024 * 1024);
        snprintf(buffer, size, "Disco: %lu MB libres de %lu MB totales", free, total);
    } else {
        snprintf(buffer, size, "Disco: Información no disponible");
    }
}

static void linux_get_network_info(unsigned long long *prev_rx, unsigned long long *prev_tx, char *buffer, size_t size) {
    FILE *fp = fopen("/proc/net/dev", "r");
    char line[256];
    unsigned long long rx = 0, tx = 0;
    if (fp) {
        fgets(line, sizeof(line), fp); // skip header
        fgets(line, sizeof(line), fp); // skip header
        while (fgets(line, sizeof(line), fp)) {
            char iface[32];
            unsigned long long iface_rx, iface_tx;
            sscanf(line, "%31s %llu %*s %*s %*s %*s %*s %*s %*s %llu", iface, &iface_rx, &iface_tx);
            if (strncmp(iface, "lo:", 3) != 0) {
                rx += iface_rx;
                tx += iface_tx;
            }
        }
        fclose(fp);
    }
    double rx_rate = (*prev_rx == 0) ? 0 : (rx - *prev_rx) / 1024.0 / 2.0;
    double tx_rate = (*prev_tx == 0) ? 0 : (tx - *prev_tx) / 1024.0 / 2.0;
    *prev_rx = rx;
    *prev_tx = tx;
    snprintf(buffer, size, "Red: %.2f KB/s recibidos | %.2f KB/s enviados", rx_rate, tx_rate);
}

static void linux_get_process_list(char *buffer, size_t size) {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    char path[256], line[256];
    size_t used = 0;
    int truncated = 0;
    buffer[0] = '\0';

    tb_append_line(buffer, size, &used, &truncated, "%-8s %-25s\n", "PID", "Nombre del Proceso");
    tb_append_line(buffer, size, &used, &truncated, "----------------------------------------\n");

    if (!dir) {
        snprintf(buffer, size, "No se pudo obtener la lista de procesos.\n");
        return;
    }

    while (!truncated && (entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
            FILE *fp = fopen(path, "r");
            if (fp) {
                char nombre[256] = "";
                while (fgets(line, sizeof(line), fp)) {
                    if (strncmp(line, "Name:", 5) == 0) {
                        sscanf(line, "Name:\t%[^\n]", nombre);
                        break;
                    }
                }
                fclose(fp);
                tb_append_line(buffer, size, &used, &truncated, "%-8s %-25s\n", entry->d_name, nombre);
            }
        }
    }
    closedir(dir);

    if (truncated) {
        snprintf(buffer + strlen(buffer), size - strlen(buffer), "... (lista truncada)\n");
    }
}

static const PlatformOps kLinuxOps = {
    .get_cpu_stats = linux_get_cpu_stats,
    .get_memory_info = linux_get_memory_info,
    .get_disk_info = linux_get_disk_info,
    .get_network_info = linux_get_network_info,
    .get_process_list = linux_get_process_list,
};

const PlatformOps *platform_get_ops(void) {
    return &kLinuxOps;
}
