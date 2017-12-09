#include <gtk/gtk.h>
#ifndef _INTERFACE_H_
#define _INTERFACE_H_

int main_interface (int argc, char **argv);
void file_chosen(GtkToolButton *filechooser, GtkImage *Displayed_image);
void saved_clicked(GtkToolButton *save, GtkTextView *Text);
void load_text(GtkTextView *text, char *path);
#endif

