#ifndef APP_H
#define APP_H

// Snapshot inmutable con todo lo que la GUI necesita mostrar en un ciclo
// de refresco. Desacopla la capa de presentación (gui.c) de cómo se
// calculan/obtienen los datos (app.c + monitor.c).
typedef struct {
    char cpu[64];
    char memory[128];
    char disk[128];
    char network[128];
    char processes[8192];
} AppSnapshot;

// Recalcula todos los indicadores y los deja listos en *snapshot.
// Mantiene internamente el estado necesario entre llamadas (últimas
// muestras de CPU y contadores de red) para no exponer variables
// globales a quien la use.
void app_refresh(AppSnapshot *snapshot);

#endif // APP_H
