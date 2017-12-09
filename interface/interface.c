# include <gtk/gtk.h>
# include <stdio.h>
# include <stdlib.h>
# include <err.h>
# include "interface.h"

char *filename;

int main_interface(int argc, char **argv)
{
    GtkWidget *window;
    GtkBuilder *builder = NULL;
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();

    gtk_builder_add_from_file (builder, "interface/test.glade", NULL);

    GtkToolButton *file_chooser = GTK_TOOL_BUTTON(
                                gtk_builder_get_object(builder,"filechooser"));
    GtkToolButton *save_button = GTK_TOOL_BUTTON(
                                gtk_builder_get_object(builder,"save"));
//    GtkTextView *text = GTK_TEXT_VIEW(gtk_builder_get_object(builder,"Text"));
    GtkToolButton *run_button =  GTK_TOOL_BUTTON(
                                gtk_builder_get_object(builder,"run"));
    gtk_builder_connect_signals(builder, save_button);
    gtk_builder_connect_signals(builder, file_chooser);
    gtk_builder_connect_signals(builder, run_button);
    window = GTK_WIDGET(gtk_builder_get_object (builder, "IAmREADy_Main"));

    gtk_builder_connect_signals(builder, NULL);

    g_object_unref( G_OBJECT( builder ) );

    gtk_widget_show_all (window);

    gtk_main();
    
//    load_text(text,"/home/martin.saconney/afs/TerminalASCII" );
//    g_print("LOAD!  LOAD! LOAD!");
    return 0;
}

void file_chosen(GtkToolButton *filechooser, GtkImage *Displayed_image){
    gtk_tool_button_set_label(filechooser, "Open");
    GtkWidget *dialog; 
    char* filename;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    dialog = gtk_file_chooser_dialog_new("Open File",
                                          NULL,
                                          action,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          ("_Open"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if(res == GTK_RESPONSE_ACCEPT){
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        gtk_image_set_from_file(Displayed_image, filename);
    }
    gtk_widget_destroy(dialog);
}

void save_clicked(GtkToolButton *save, GtkTextView *Text){
    gtk_tool_button_set_label(save,"Save");
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    //char *filename;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
    dialog = gtk_file_chooser_dialog_new("Save File",
                                          NULL,
                                          action,
                                          ("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          ("_Save"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);
    chooser = GTK_FILE_CHOOSER(dialog);
/*  if(user_edited_new_document)
        gtk_file_chooser_set_current_name(chooser,("New document"));
    free(string);
    else
        gtk_file_chooser_ser_filename(chooser, existing_filename);
*/  res = gtk_dialog_run (GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT){
        filename = gtk_file_chooser_get_filename(chooser);
//        save_to_file(filename);
          FILE *file = fopen(filename,"w");

          GtkTextBuffer *buffer = gtk_text_view_get_buffer(Text);

          GtkTextIter start;
          GtkTextIter end;
          GError *err = NULL;

          gtk_text_buffer_get_start_iter (buffer, &start);
          gtk_text_buffer_get_end_iter (buffer, &end);

          gchar *text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
          g_file_set_contents (filename, text, -1, &err);
          
          //g_print((char*)buffer);
          //fprintf(file,"%s",(char*)buffer);
          fclose(file);
          g_free (text);
          g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void on_run_clicked(GtkToolButton *run, GtkTextView *text){
    g_print("YUP!\n");
    gtk_tool_button_set_label(run, "Run");
    g_print("lel\n");
    load_text(text,"interface/test.nothing");
    g_print("GG");
}

void load_text(GtkTextView *text, char* path){
    FILE *file = fopen(path,"r");
    char string[1024];
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text);
    int i = 0;
    while(fgets(string, 1024, file) != NULL){
        i+=1;
        //fgets(string, 1024, file);
        gtk_text_buffer_set_text(buffer, string, -1);
        g_print("%s\n",string);
    }
    fclose(file);
}
