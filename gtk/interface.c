/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

static GnomeUIInfo file1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_EXIT_ITEM (NULL, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo view1_menu_uiinfo[] =
{
  GNOMEUIINFO_END
};

static GnomeUIInfo help1_menu_uiinfo[] =
{
  GNOMEUIINFO_MENU_ABOUT_ITEM (NULL, NULL),
  GNOMEUIINFO_END
};

static GnomeUIInfo menubar1_uiinfo[] =
{
  GNOMEUIINFO_MENU_FILE_TREE (file1_menu_uiinfo),
  GNOMEUIINFO_MENU_VIEW_TREE (view1_menu_uiinfo),
  GNOMEUIINFO_MENU_HELP_TREE (help1_menu_uiinfo),
  GNOMEUIINFO_END
};

GtkWidget*
create_main_win (void)
{
  GtkWidget *main_win;
  GtkWidget *dock1;
  GtkWidget *toolbar1;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *open_button;
  GtkWidget *remove_button;
  GtkWidget *scrolledwindow1;
  GtkWidget *clist3;
  GtkWidget *name_label;
  GtkWidget *filename_label;
  GtkWidget *shields_label;
  GtkWidget *appbar1;

  main_win = gnome_app_new ("nobots", _("NoBots"));
  gtk_object_set_data (GTK_OBJECT (main_win), "main_win", main_win);
  gtk_window_set_default_size (GTK_WINDOW (main_win), 400, 350);

  dock1 = GNOME_APP (main_win)->dock;
  gtk_widget_ref (dock1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "dock1", dock1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (dock1);

  gnome_app_create_menus (GNOME_APP (main_win), menubar1_uiinfo);

  gtk_widget_ref (menubar1_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "file1",
                            menubar1_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (file1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "exit1",
                            file1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (menubar1_uiinfo[1].widget);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "view1",
                            menubar1_uiinfo[1].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (menubar1_uiinfo[2].widget);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "help1",
                            menubar1_uiinfo[2].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_widget_ref (help1_menu_uiinfo[0].widget);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "about1",
                            help1_menu_uiinfo[0].widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_BOTH);
  gtk_widget_ref (toolbar1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "toolbar1", toolbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar1);
  gnome_app_add_toolbar (GNOME_APP (main_win), GTK_TOOLBAR (toolbar1), "toolbar1",
                                GNOME_DOCK_ITEM_BEH_EXCLUSIVE,
                                GNOME_DOCK_TOP, 1, 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (toolbar1), 1);
  gtk_toolbar_set_space_size (GTK_TOOLBAR (toolbar1), 16);
  gtk_toolbar_set_space_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_SPACE_LINE);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar1), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (main_win, GNOME_STOCK_PIXMAP_OPEN);
  open_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Open"),
                                _("�ppna fil"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (open_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "open_button", open_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (open_button);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (main_win, GNOME_STOCK_PIXMAP_TRASH);
  remove_button = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Remove"),
                                _("Spara fil"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (remove_button);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "remove_button", remove_button,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (remove_button);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow1);
  gnome_app_set_contents (GNOME_APP (main_win), scrolledwindow1);

  clist3 = gtk_clist_new (3);
  gtk_widget_ref (clist3);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "clist3", clist3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (clist3);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), clist3);
  gtk_clist_set_column_width (GTK_CLIST (clist3), 0, 80);
  gtk_clist_set_column_width (GTK_CLIST (clist3), 1, 166);
  gtk_clist_set_column_width (GTK_CLIST (clist3), 2, 80);
  gtk_clist_column_titles_show (GTK_CLIST (clist3));

  name_label = gtk_label_new (_("Name"));
  gtk_widget_ref (name_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "name_label", name_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (name_label);
  gtk_clist_set_column_widget (GTK_CLIST (clist3), 0, name_label);

  filename_label = gtk_label_new (_("File name"));
  gtk_widget_ref (filename_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "filename_label", filename_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (filename_label);
  gtk_clist_set_column_widget (GTK_CLIST (clist3), 1, filename_label);

  shields_label = gtk_label_new (_("Shields"));
  gtk_widget_ref (shields_label);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "shields_label", shields_label,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (shields_label);
  gtk_clist_set_column_widget (GTK_CLIST (clist3), 2, shields_label);

  appbar1 = gnome_appbar_new (TRUE, TRUE, GNOME_PREFERENCES_NEVER);
  gtk_widget_ref (appbar1);
  gtk_object_set_data_full (GTK_OBJECT (main_win), "appbar1", appbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (appbar1);
  gnome_app_set_statusbar (GNOME_APP (main_win), appbar1);

  gnome_app_install_menu_hints (GNOME_APP (main_win), menubar1_uiinfo);
  gtk_signal_connect (GTK_OBJECT (open_button), "clicked",
                      GTK_SIGNAL_FUNC (open_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (remove_button), "clicked",
                      GTK_SIGNAL_FUNC (remove_clicked),
                      NULL);

  return main_win;
}
