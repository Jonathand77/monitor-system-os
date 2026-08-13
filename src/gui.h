#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "app.h"

// Referencias a los widgets que se actualizan en cada refresco.
typedef struct {
    GtkWidget *cpu_label;
    GtkWidget *mem_label;
    GtkWidget *disk_label;
    GtkWidget *net_label;
    GtkTextBuffer *proc_buffer;
} GuiWidgets;

// Construye la ventana principal y rellena *widgets con los punteros
// necesarios para refrescarla luego con gui_update().
GtkWidget *gui_build_window(GuiWidgets *widgets);

// Vuelca un AppSnapshot en los widgets ya construidos.
void gui_update(GuiWidgets *widgets, const AppSnapshot *snapshot);

#endif // GUI_H
