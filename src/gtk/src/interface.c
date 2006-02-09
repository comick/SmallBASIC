/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget      *
create_main_window(void)
{
    GtkWidget      *main_window;
    GtkWidget      *vbox1;
    GtkWidget      *menubar;
    GtkWidget      *menuitem1;
    GtkWidget      *menuitem1_menu;
    GtkWidget      *stop;
    GtkWidget      *help1;
    GtkWidget      *separator1;
    GtkWidget      *about;
    GtkWidget      *separatormenuitem1;
    GtkWidget      *quit1;
    GtkWidget      *drawing_area;
    GtkWidget      *statusbar;
    GtkAccelGroup  *accel_group;

    accel_group = gtk_accel_group_new();

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(main_window, "main_window");
    gtk_window_set_title(GTK_WINDOW(main_window), _("SmallBASIC"));

    vbox1 = gtk_vbox_new(FALSE, 0);
    gtk_widget_set_name(vbox1, "vbox1");
    gtk_widget_show(vbox1);
    gtk_container_add(GTK_CONTAINER(main_window), vbox1);

    menubar = gtk_menu_bar_new();
    gtk_widget_set_name(menubar, "menubar");
    gtk_widget_show(menubar);
    gtk_box_pack_start(GTK_BOX(vbox1), menubar, FALSE, FALSE, 0);

    menuitem1 = gtk_menu_item_new_with_mnemonic(_("_File"));
    gtk_widget_set_name(menuitem1, "menuitem1");
    gtk_widget_show(menuitem1);
    gtk_container_add(GTK_CONTAINER(menubar), menuitem1);

    menuitem1_menu = gtk_menu_new();
    gtk_widget_set_name(menuitem1_menu, "menuitem1_menu");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem1), menuitem1_menu);

    stop = gtk_image_menu_item_new_from_stock("gtk-stop", accel_group);
    gtk_widget_set_name(stop, "stop");
    gtk_widget_show(stop);
    gtk_container_add(GTK_CONTAINER(menuitem1_menu), stop);

    help1 = gtk_image_menu_item_new_from_stock("gtk-help", accel_group);
    gtk_widget_set_name(help1, "help1");
    gtk_widget_show(help1);
    gtk_container_add(GTK_CONTAINER(menuitem1_menu), help1);

    separator1 = gtk_separator_menu_item_new();
    gtk_widget_set_name(separator1, "separator1");
    gtk_widget_show(separator1);
    gtk_container_add(GTK_CONTAINER(menuitem1_menu), separator1);
    gtk_widget_set_sensitive(separator1, FALSE);

    about = gtk_image_menu_item_new_from_stock("gtk-about", accel_group);
    gtk_widget_set_name(about, "about");
    gtk_widget_show(about);
    gtk_container_add(GTK_CONTAINER(menuitem1_menu), about);

    separatormenuitem1 = gtk_separator_menu_item_new();
    gtk_widget_set_name(separatormenuitem1, "separatormenuitem1");
    gtk_widget_show(separatormenuitem1);
    gtk_container_add(GTK_CONTAINER(menuitem1_menu), separatormenuitem1);
    gtk_widget_set_sensitive(separatormenuitem1, FALSE);

    quit1 = gtk_image_menu_item_new_from_stock("gtk-quit", accel_group);
    gtk_widget_set_name(quit1, "quit1");
    gtk_widget_show(quit1);
    gtk_container_add(GTK_CONTAINER(menuitem1_menu), quit1);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_name(drawing_area, "drawing_area");
    gtk_widget_show(drawing_area);
    gtk_box_pack_start(GTK_BOX(vbox1), drawing_area, TRUE, TRUE, 0);
    gtk_widget_set_size_request(drawing_area, 200, 240);

    statusbar = gtk_statusbar_new();
    gtk_widget_set_name(statusbar, "statusbar");
    gtk_widget_show(statusbar);
    gtk_box_pack_start(GTK_BOX(vbox1), statusbar, FALSE, FALSE, 0);

    g_signal_connect((gpointer) stop, "activate",
		     G_CALLBACK(on_stop_activate), NULL);
    g_signal_connect((gpointer) help1, "activate",
		     G_CALLBACK(on_help1_activate), NULL);
    g_signal_connect((gpointer) about, "activate",
		     G_CALLBACK(on_about_activate), NULL);
    g_signal_connect((gpointer) quit1, "activate",
		     G_CALLBACK(on_quit1_activate), NULL);

    /*
     * Store pointers to all widgets, for use by lookup_widget(). 
     */
    GLADE_HOOKUP_OBJECT_NO_REF(main_window, main_window, "main_window");
    GLADE_HOOKUP_OBJECT(main_window, vbox1, "vbox1");
    GLADE_HOOKUP_OBJECT(main_window, menubar, "menubar");
    GLADE_HOOKUP_OBJECT(main_window, menuitem1, "menuitem1");
    GLADE_HOOKUP_OBJECT(main_window, menuitem1_menu, "menuitem1_menu");
    GLADE_HOOKUP_OBJECT(main_window, stop, "stop");
    GLADE_HOOKUP_OBJECT(main_window, help1, "help1");
    GLADE_HOOKUP_OBJECT(main_window, separator1, "separator1");
    GLADE_HOOKUP_OBJECT(main_window, about, "about");
    GLADE_HOOKUP_OBJECT(main_window, separatormenuitem1,
			"separatormenuitem1");
    GLADE_HOOKUP_OBJECT(main_window, quit1, "quit1");
    GLADE_HOOKUP_OBJECT(main_window, drawing_area, "drawing_area");
    GLADE_HOOKUP_OBJECT(main_window, statusbar, "statusbar");

    gtk_window_add_accel_group(GTK_WINDOW(main_window), accel_group);

    return main_window;
}

GtkWidget      *
create_aboutdialog(void)
{
    GtkWidget      *aboutdialog;
    /*
     * TRANSLATORS: Replace this string with your names, one name per
     * line. 
     */
    gchar          *translators = _("translator-credits");
    GdkPixbuf      *aboutdialog_logo_pixbuf;

    aboutdialog = gtk_about_dialog_new();
    gtk_widget_set_name(aboutdialog, "aboutdialog");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(aboutdialog), VERSION);
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(aboutdialog),
			      _("SmallBASIC"));
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(aboutdialog),
				 "smallbasic.sf.net");
    gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(aboutdialog),
					    translators);
    aboutdialog_logo_pixbuf = create_pixbuf("sb32x32.png");
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(aboutdialog),
			      aboutdialog_logo_pixbuf);

    /*
     * Store pointers to all widgets, for use by lookup_widget(). 
     */
    GLADE_HOOKUP_OBJECT_NO_REF(aboutdialog, aboutdialog, "aboutdialog");

    return aboutdialog;
}
