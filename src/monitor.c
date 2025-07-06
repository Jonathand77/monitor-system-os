#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "monitor.h"

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif

// --- MEMORIA ---
void mostrar_memoria(char *buffer, size_t size) {
#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        unsigned long total = (unsigned long)(statex.ullTotalPhys / (1024 * 1024));
        unsigned long avail = (unsigned long)(statex.ullAvailPhys / (1024 * 1024));
        snprintf(buffer, size, "RAM: %lu MB disponibles / %lu MB totales", avail, total);
    } else {
        snprintf(buffer, size, "RAM: Información no disponible");
    }
#else
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
#endif
}

// --- DISCO ---
void mostrar_disco(char *buffer, size_t size) {
#ifdef _WIN32
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        unsigned long total = (unsigned long)(totalNumberOfBytes.QuadPart / (1024 * 1024));
        unsigned long free = (unsigned long)(totalNumberOfFreeBytes.QuadPart / (1024 * 1024));
        snprintf(buffer, size, "Disco: %lu MB libres de %lu MB totales", free, total);
    } else {
        snprintf(buffer, size, "Disco: Información no disponible");
    }
#else
    #include <sys/statvfs.h>
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        unsigned long total = (stat.f_blocks * stat.f_frsize) / (1024 * 1024);
        unsigned long free = (stat.f_bfree * stat.f_frsize) / (1024 * 1024);
        snprintf(buffer, size, "Disco: %lu MB libres de %lu MB totales", free, total);
    } else {
        snprintf(buffer, size, "Disco: Información no disponible");
    }
#endif
}

// --- RED ---
void mostrar_red(unsigned long long *prev_rx, unsigned long long *prev_tx, char *buffer, size_t size) {
#ifdef _WIN32
    // Usar GetIfTable2 para obtener tráfico de red
    MIB_IF_TABLE2 *ifTable;
    ULONG rx = 0, tx = 0;
    if (GetIfTable2(&ifTable) == NO_ERROR) {
        for (ULONG i = 0; i < ifTable->NumEntries; ++i) {
            MIB_IF_ROW2 *row = &ifTable->Table[i];
            // Solo interfaces operativas y no loopback
            if (row->OperStatus == IfOperStatusUp && !(row->Type == IF_TYPE_SOFTWARE_LOOPBACK)) {
                rx += (ULONG)row->InOctets;
                tx += (ULONG)row->OutOctets;
            }
        }
        FreeMibTable(ifTable);
    }
    double rx_rate = (*prev_rx == 0) ? 0 : (rx - *prev_rx) / 1024.0 / 2.0;
    double tx_rate = (*prev_tx == 0) ? 0 : (tx - *prev_tx) / 1024.0 / 2.0;
    *prev_rx = rx;
    *prev_tx = tx;
    snprintf(buffer, size, "Red: %.2f KB/s recibidos | %.2f KB/s enviados", rx_rate, tx_rate);
#else
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
#endif
}

// --- CPU ---
CpuStats get_cpu_stats() {
    CpuStats stats = {0};
#ifdef _WIN32
    // Usar GetSystemTimes
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER idle, kernel, user;
        idle.LowPart = idleTime.dwLowDateTime;
        idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;
        stats.idle = idle.QuadPart;
        stats.system = kernel.QuadPart - idle.QuadPart;
        stats.user = user.QuadPart;
        // Otros campos no disponibles en Windows
    }
#else
    FILE *fp = fopen("/proc/stat", "r");
    if (fp) {
        fscanf(fp, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
               &stats.user, &stats.nice, &stats.system, &stats.idle,
               &stats.iowait, &stats.irq, &stats.softirq, &stats.steal);
        fclose(fp);
    }
#endif
    return stats;
}

double calculate_cpu_usage(CpuStats prev, CpuStats curr) {
#ifdef _WIN32
    unsigned long long prev_idle = prev.idle;
    unsigned long long curr_idle = curr.idle;
    unsigned long long prev_total = prev.user + prev.system + prev.idle;
    unsigned long long curr_total = curr.user + curr.system + curr.idle;
    double totald = (double)(curr_total - prev_total);
    double idled = (double)(curr_idle - prev_idle);
    if (totald == 0) return 0.0;
    return 100.0 * (totald - idled) / totald;
#else
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

    if (totald == 0) return 0.0;
    return 100.0 * (totald - idled) / totald;
#endif
}

// --- PROCESOS ---
void mostrar_procesos(char *buffer, size_t size) {
#ifdef _WIN32
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    size_t used = 0;
    buffer[0] = '\0';
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        snprintf(buffer, size, "No se pudo obtener la lista de procesos.\n");
        return;
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        snprintf(buffer, size, "No se pudo obtener la lista de procesos.\n");
        return;
    }
    used += snprintf(buffer + used, size - used, "%-8s %-25s\n", "PID", "Nombre del Proceso");
    used += snprintf(buffer + used, size - used, "----------------------------------------\n");
    do {
        used += snprintf(buffer + used, size - used, "%-8lu %-25ls\n", pe32.th32ProcessID, pe32.szExeFile);
        if (used >= size - 64) break; // Evita desbordamiento
    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
#else
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    char path[256], line[256];
    size_t used = 0;
    buffer[0] = '\0';

    used += snprintf(buffer + used, size - used, "%-8s %-25s\n", "PID", "Nombre del Proceso");
    used += snprintf(buffer + used, size - used, "----------------------------------------\n");

    while ((entry = readdir(dir)) != NULL) {
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
                used += snprintf(buffer + used, size - used, "%-8s %-25s\n", entry->d_name, nombre);
                if (used >= size - 64) break; // Evita desbordamiento
            }
        }
    }
    closedir(dir);
#endif
}