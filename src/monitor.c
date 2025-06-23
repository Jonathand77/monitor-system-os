#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "monitor.h"

#ifdef _WIN32
#include <windows.h>
#endif

// Function to update the GUI with the latest monitoring data
void update_gui() {
    // Here you would implement the logic to update the GUI with the latest stats
    // For example, you could call functions from monitor.c to get the latest CPU, memory, disk, and network usage
}

// Function to initialize the monitoring process
void start_monitoring() {
    while (1) {
        // Call the update function to refresh the GUI
        update_gui();
        
        // Sleep for a specified interval before updating again
        sleep(3);
    }
}

// Ejemplo de implementación para memoria
void mostrar_memoria(char *buffer, size_t size) {
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

void mostrar_red(unsigned long long *prev_rx, unsigned long long *prev_tx, char *buffer, size_t size) {
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
            // Solo sumar interfaces que no sean loopback
            if (strncmp(iface, "lo:", 3) != 0) {
                rx += iface_rx;
                tx += iface_tx;
            }
        }
        fclose(fp);
    }
    double rx_rate = (*prev_rx == 0) ? 0 : (rx - *prev_rx) / 1024.0 / 2.0; // KB/s (asumiendo refresh cada 2s)
    double tx_rate = (*prev_tx == 0) ? 0 : (tx - *prev_tx) / 1024.0 / 2.0;
    *prev_rx = rx;
    *prev_tx = tx;
    snprintf(buffer, size, "Red: %.2f KB/s recibidos | %.2f KB/s enviados", rx_rate, tx_rate);
}

// Main function to initialize the GUI and start monitoring
int main() {
    // Initialize the GUI
    // This would typically involve creating a window and setting up the layout
    // For example, you might call a function from gui.c to create the window

    // Start the monitoring process
    start_monitoring();

    return 0;
}