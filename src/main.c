#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "monitor.h"

static CpuStats prev_cpu_stats = {0};
static unsigned long long prev_rx = 0, prev_tx = 0;

static void update_display(GtkWidget *cpu_label, GtkWidget *mem_label, GtkWidget *disk_label, GtkWidget *net_label) {
    // CPU
    CpuStats curr_cpu_stats = get_cpu_stats();
    double cpu_usage = calculate_cpu_usage(prev_cpu_stats, curr_cpu_stats);
    prev_cpu_stats = curr_cpu_stats;
    char cpu_text[64];
    snprintf(cpu_text, sizeof(cpu_text), "Uso de CPU: %.2f%%", cpu_usage);
    gtk_label_set_text(GTK_LABEL(cpu_label), cpu_text);

    // Memoria
    char mem_text[128];
    mostrar_memoria(mem_text, sizeof(mem_text));
    gtk_label_set_text(GTK_LABEL(mem_label), mem_text);

    // Disco
    char disk_text[128];
    mostrar_disco(disk_text, sizeof(disk_text));
    gtk_label_set_text(GTK_LABEL(disk_label), disk_text);

    // Red
    char net_text[128];
    mostrar_red(&prev_rx, &prev_tx, net_text, sizeof(net_text));
    gtk_label_set_text(GTK_LABEL(net_label), net_text);
}

static gboolean refresh_data(gpointer user_data) {
    GtkWidget **labels = (GtkWidget **)user_data;
    update_display(labels[0], labels[1], labels[2], labels[3]);
    return TRUE; // Sigue llamando periódicamente
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Monitor del Sistema");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *cpu_label = gtk_label_new("Uso de CPU: 0.00%");
    GtkWidget *mem_label = gtk_label_new("RAM: 0 MB / 0 MB disponibles");
    GtkWidget *disk_label = gtk_label_new("Disco: 0 MB libres de 0 MB totales");
    GtkWidget *net_label = gtk_label_new("Red: 0 KB/s recibidos | 0 KB/s enviados");

    gtk_grid_attach(GTK_GRID(grid), cpu_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mem_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), disk_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), net_label, 0, 3, 1, 1);

    GtkWidget *labels[] = {cpu_label, mem_label, disk_label, net_label};
    g_timeout_add_seconds(2, refresh_data, labels);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}