#include <gtk/gtk.h>
#include "monitor.h"

static void update_display(GtkLabel *cpu_label, GtkLabel *mem_label, GtkLabel *disk_label, GtkLabel *net_label) {
    CpuStats cpu_stats = get_cpu_stats();
    mostrar_memoria();
    mostrar_disco();
    mostrar_red();

    char cpu_usage_text[50];
    snprintf(cpu_usage_text, sizeof(cpu_usage_text), "Uso de CPU: %.2f%%", calculate_cpu_usage(prev_stats, cpu_stats));
    gtk_label_set_text(cpu_label, cpu_usage_text);

    // Update memory, disk, and network labels similarly
    // For example:
    // gtk_label_set_text(mem_label, "RAM: ...");
    // gtk_label_set_text(disk_label, "Disco: ...");
    // gtk_label_set_text(net_label, "Red: ...");
}

static gboolean refresh_data(gpointer user_data) {
    GtkLabel **labels = (GtkLabel **)user_data;
    update_display(labels[0], labels[1], labels[2], labels[3]);
    return TRUE; // Continue calling this function
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Monitor del Sistema");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkLabel *cpu_label = GTK_LABEL(gtk_label_new("Uso de CPU: 0.00%"));
    GtkLabel *mem_label = GTK_LABEL(gtk_label_new("RAM: 0 MB / 0 MB disponibles"));
    GtkLabel *disk_label = GTK_LABEL(gtk_label_new("Disco: 0 MB libres de 0 MB totales"));
    GtkLabel *net_label = GTK_LABEL(gtk_label_new("Red: 0 KB/s recibidos | 0 KB/s enviados"));

    gtk_grid_attach(GTK_GRID(grid), cpu_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mem_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), disk_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), net_label, 0, 3, 1, 1);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Set up a timer to refresh the data every 3 seconds
    GtkLabel *labels[] = {cpu_label, mem_label, disk_label, net_label};
    g_timeout_add(3000, refresh_data, labels);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}