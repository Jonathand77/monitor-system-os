#include "gui.h"

GtkWidget *gui_build_window(GuiWidgets *widgets) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Monitor del Sistema");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    widgets->cpu_label = gtk_label_new("Uso de CPU: 0.00%");
    widgets->mem_label = gtk_label_new("RAM: 0 MB / 0 MB disponibles");
    widgets->disk_label = gtk_label_new("Disco: 0 MB libres de 0 MB totales");
    widgets->net_label = gtk_label_new("Red: 0 KB/s recibidos | 0 KB/s enviados");

    GtkWidget *proc_view = gtk_text_view_new();
    widgets->proc_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(proc_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(proc_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(proc_view), FALSE);

    gtk_grid_attach(GTK_GRID(grid), widgets->cpu_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->mem_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->disk_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->net_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), proc_view, 0, 4, 1, 1);

    return window;
}

void gui_update(GuiWidgets *widgets, const AppSnapshot *snapshot) {
    gtk_label_set_text(GTK_LABEL(widgets->cpu_label), snapshot->cpu);
    gtk_label_set_text(GTK_LABEL(widgets->mem_label), snapshot->memory);
    gtk_label_set_text(GTK_LABEL(widgets->disk_label), snapshot->disk);
    gtk_label_set_text(GTK_LABEL(widgets->net_label), snapshot->network);
    gtk_text_buffer_set_text(widgets->proc_buffer, snapshot->processes, -1);
}
