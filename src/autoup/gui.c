#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "config.h"
#include "autoup/common.h"

#define BUFF 256

//Returns the stdout from the executed process
char *openProc(char *command)
{
    FILE *proc;
    int read, len, size;
    char *ret, *tmp, buff[BUFF];

    proc = popen(command, "r");
    if(proc == NULL)
        return NULL;
    size = BUFF;
    ret = (char*)malloc(size);
    len = 0;
    ret[len] = 0;
    while(read = fread(buff, 1, BUFF, proc))
    {
        if(size <= read + len)
        {
            while(size <= read+len) size<<=1;
            tmp = (char*)malloc(size);
            memcpy(tmp, ret, len+1);
            free(ret);
            ret = tmp;
        }
        memcpy(ret+len, buff, read);
        len += read;
        ret[len] = 0;
    }
    pclose(proc);
    return ret;
}
void reboot(GtkWidget *widget, gpointer data)
{
    char *tmp;
    g_print("Rebooting!\n");
    //tmp = openProc("dbus-send --system --print-reply --dest=org.freedesktop.ConsoleKit /org/freedesktop/ConsoleKit/Manager org.freedesktop.ConsoleKit.Manager.Restart");
    tmp = openProc("/usr/bin/sudo /sbin/shutdown -r now");
    if(tmp != NULL) free(tmp);
}
gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return TRUE;
}
void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}
GtkWidget *makeButton(const char *label, const gchar *img)
{
    GtkWidget *button, *box, *lbl, *image;

    //Generate new button
    button = gtk_button_new();

    //Create Box
    box = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(box), 2);

    //Create Label
    lbl = gtk_label_new(label);
    gtk_widget_show(lbl);

    //Create Image
    image = gtk_image_new_from_stock(img, GTK_ICON_SIZE_MENU);
    gtk_widget_show(image);

    //Add Contents to Box
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 3);
    gtk_box_pack_start(GTK_BOX(box), lbl, TRUE, TRUE, 3);

    //Add box to button
    gtk_widget_show(box);
    gtk_container_add(GTK_CONTAINER(button), box);

    return button;
}
void displayReboot(int argc, char **argv)
{
    GtkWidget *window;
    GtkWidget *yes, *no, *label;
    GtkWidget *hbox, *vbox;

    //Initialize Gtk
    gtk_init(&argc, &argv);

    //Setup Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);
    gtk_window_set_title(window, "Automatic Updates");
    gtk_window_set_resizable(window, FALSE);
    gtk_window_set_modal(window, TRUE);
    gtk_window_set_keep_above(window, TRUE);
    gtk_window_set_deletable(window, FALSE);
    gtk_window_set_type_hint(window, GDK_WINDOW_TYPE_HINT_MENU);

    //Set Window Callbacks
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);

    //Create VBox
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);

    //Create HBox
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);

    //Create Label
    label = gtk_label_new("Updates have been successfully installed on your system.\nWould you like to restart now?");
    gtk_label_set_justify(label, GTK_JUSTIFY_CENTER);
    gtk_widget_show(label);

    //Init Yes Button
    yes = makeButton("Restart Now", GTK_STOCK_OPEN);
    g_signal_connect(yes, "clicked", G_CALLBACK(reboot), NULL);
    g_signal_connect_swapped(yes, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_widget_show(yes);

    //Init No Button
    no = makeButton("Restart Later", GTK_STOCK_OPEN);
    g_signal_connect_swapped(no, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_widget_show(no);

    //Add Elements to HBox
    gtk_box_pack_start(GTK_BOX(hbox), yes, TRUE, TRUE, 3);
    gtk_box_pack_start(GTK_BOX(hbox), no, TRUE, TRUE, 3);
    gtk_widget_show(hbox);

    //Add Elements to VBox
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 3);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);
    gtk_widget_show(vbox);

    //Add Elements to Window
    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show(window);

    //Run
    gtk_main();
}

int main(int argc, char **argv)
{
    FILE *updated;
    char *out;
    int num;

    //Set X Display
    setenv("DISPLAY", ":0", 1);

    //Try to open the file
    updated = fopen(UPDATE_FILE, "r");
    if(updated == NULL)
    {
        printf("Failed to get update file / No update found.\n");
        return EXIT_FAILURE;
    }

    //Read the first character to test if it was updated
    num = fgetc(updated);
    fclose(updated);
    if(num != '1')
    {
        printf("Not updated\n");
        return EXIT_SUCCESS;
    }

    //Rewrite to the file to record our check
    updated = fopen(UPDATE_FILE, "w");
    if(updated == NULL)
    {
        printf("Failed to update the update file\n");
        return EXIT_FAILURE;
    }
    fputc('0', updated);
    fclose(updated);

    //If new packages were installed ask for reboot
    displayReboot(argc, argv);
    printf("Installed new packages.\nPlease Reboot your system.\n");
    return EXIT_SUCCESS;
}
