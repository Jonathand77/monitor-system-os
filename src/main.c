#include <gtk/gtk.h>
#include "app.h"
#include "gui.h"

static GuiWidgets g_widgets;

static gboolean on_timer_tick(gpointer user_data) {
    (void)user_data;
    AppSnapshot snapshot;
    app_refresh(&snapshot);
    gui_update(&g_widgets, &snapshot);
    return TRUE; // Sigue llamando periódicamente
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gui_build_window(&g_widgets);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    g_timeout_add_seconds(2, on_timer_tick, NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
