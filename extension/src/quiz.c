#include <gtk/gtk.h>

  GtkWidget *g_welcome;
  GtkWidget *more_info;
  



int main(int argc, char *argv[]) {
    GtkBuilder *builder; 
    GtkWidget *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "main_window_glade.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);
    g_welcome = GTK_WIDGET(gtk_builder_get_object(builder, "welcome"));
    more_info = GTK_WIDGET(gtk_builder_get_object(builder, "more_information"));

    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();

    return 0;
}

void on_window_main_destroy(void) {
    gtk_main_quit();
}

void open_page(void) {
  system("open https://blacklivesmatters.carrd.co/");
}

// called when window is closed



// to compile gcc testprogram2.c -o base `pkg-config --cflags --libs gtk+-3.0`