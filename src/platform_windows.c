#include "platform.h"
#include "text_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <netioapi.h>
#pragma comment(lib, "iphlpapi.lib")

static CpuStats windows_get_cpu_stats(void) {
    CpuStats stats = {0};
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
        // kernelTime de Windows incluye el tiempo idle; se resta para que
        // "system" represente solo tiempo de kernel no-idle, igual que en Linux.
        stats.system = kernel.QuadPart - idle.QuadPart;
        stats.user = user.QuadPart;
        // nice/iowait/irq/softirq/steal no están disponibles en Windows y
        // quedan en 0 (calculate_cpu_usage está pensado para tolerarlo).
    }
    return stats;
}

static void windows_get_memory_info(char *buffer, size_t size) {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        unsigned long total = (unsigned long)(statex.ullTotalPhys / (1024 * 1024));
        unsigned long avail = (unsigned long)(statex.ullAvailPhys / (1024 * 1024));
        snprintf(buffer, size, "RAM: %lu MB disponibles / %lu MB totales", avail, total);
    } else {
        snprintf(buffer, size, "RAM: Información no disponible");
    }
}

static void windows_get_disk_info(char *buffer, size_t size) {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        unsigned long total = (unsigned long)(totalNumberOfBytes.QuadPart / (1024 * 1024));
        unsigned long free = (unsigned long)(totalNumberOfFreeBytes.QuadPart / (1024 * 1024));
        snprintf(buffer, size, "Disco: %lu MB libres de %lu MB totales", free, total);
    } else {
        snprintf(buffer, size, "Disco: Información no disponible");
    }
}

static void windows_get_network_info(unsigned long long *prev_rx, unsigned long long *prev_tx, char *buffer, size_t size) {
    MIB_IFTABLE *ifTable;
    DWORD dwSize = 0;
    ULONG rx = 0, tx = 0;

    GetIfTable(NULL, &dwSize, FALSE);
    ifTable = (MIB_IFTABLE *) malloc(dwSize);
    if (ifTable == NULL) {
        snprintf(buffer, size, "Red: Error de memoria");
        return;
    }
    if (GetIfTable(ifTable, &dwSize, FALSE) == NO_ERROR) {
        for (DWORD i = 0; i < ifTable->dwNumEntries; i++) {
            MIB_IFROW *row = &ifTable->table[i];
            if (row->dwOperStatus == IF_OPER_STATUS_OPERATIONAL && !(row->dwType == IF_TYPE_SOFTWARE_LOOPBACK)) {
                rx += row->dwInOctets;
                tx += row->dwOutOctets;
            }
        }
    }
    free(ifTable);

    double rx_rate = (*prev_rx == 0) ? 0 : (rx - *prev_rx) / 1024.0 / 2.0;
    double tx_rate = (*prev_tx == 0) ? 0 : (tx - *prev_tx) / 1024.0 / 2.0;
    *prev_rx = rx;
    *prev_tx = tx;
    snprintf(buffer, size, "Red: %.2f KB/s recibidos | %.2f KB/s enviados", rx_rate, tx_rate);
}

static void windows_get_process_list(char *buffer, size_t size) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    size_t used = 0;
    int truncated = 0;
    buffer[0] = '\0';

    tb_append_line(buffer, size, &used, &truncated, "%-8s %-25s\n", "PID", "Nombre del Proceso");
    tb_append_line(buffer, size, &used, &truncated, "----------------------------------------\n");

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
    do {
        tb_append_line(buffer, size, &used, &truncated, "%-8lu %-25s\n", pe32.th32ProcessID, pe32.szExeFile);
    } while (!truncated && Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);

    if (truncated) {
        snprintf(buffer + strlen(buffer), size - strlen(buffer), "... (lista truncada)\n");
    }
}

static const PlatformOps kWindowsOps = {
    .get_cpu_stats = windows_get_cpu_stats,
    .get_memory_info = windows_get_memory_info,
    .get_disk_info = windows_get_disk_info,
    .get_network_info = windows_get_network_info,
    .get_process_list = windows_get_process_list,
};

const PlatformOps *platform_get_ops(void) {
    return &kWindowsOps;
}
