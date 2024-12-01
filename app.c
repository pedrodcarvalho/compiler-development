#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_reader.h"

#define CODE_FILE "input.txt"
#define OBJ_FILE "output.obj"

typedef struct {
    GtkTextBuffer *text_buffer;
    GtkTextView *editor_view;
    GtkTextView *line_numbers_view;
    GtkWidget *error_label;
    GtkWidget *cursor_position_label; // Add a label for cursor position
} EditorComponents;

void update_line_numbers(GtkTextBuffer *buffer, gpointer user_data)
{
    EditorComponents *editor_components = (EditorComponents *)user_data;
    GtkTextBuffer *line_buffer = gtk_text_view_get_buffer(editor_components->line_numbers_view);

    gtk_text_buffer_set_text(line_buffer, "", -1);

    int num_lines = gtk_text_buffer_get_line_count(buffer);
    GString *line_numbers = g_string_new(NULL);

    for (int i = 1; i <= num_lines; i++) {
        g_string_append_printf(line_numbers, "%d\n", i);
    }

    gtk_text_buffer_set_text(line_buffer, line_numbers->str, -1);
    g_string_free(line_numbers, TRUE);
}

void synchronize_scroll(GtkAdjustment *adj, gpointer user_data)
{
    EditorComponents *editor_components = (EditorComponents *)user_data;
    GtkAdjustment *line_adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(editor_components->line_numbers_view));
    gtk_adjustment_set_value(line_adj, gtk_adjustment_get_value(adj));
}

void update_cursor_position(GtkTextBuffer *buffer, gpointer user_data)
{
    EditorComponents *editor_components = (EditorComponents *)user_data;

    GtkTextIter cursor_iter;
    GtkTextMark *insert_mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_buffer_get_iter_at_mark(buffer, &cursor_iter, insert_mark);

    gint line = gtk_text_iter_get_line(&cursor_iter) + 1;       // Line numbers are 1-based
    gint col = gtk_text_iter_get_line_offset(&cursor_iter) + 1; // Column numbers are 1-based

    gchar *cursor_position_text = g_strdup_printf("Line: %d, Col: %d", line, col);
    gtk_label_set_text(GTK_LABEL(editor_components->cursor_position_label), cursor_position_text);

    g_free(cursor_position_text);
}

void save_to_file(GtkTextBuffer *buffer)
{
    GtkTextIter start, end;
    gchar *text;

    gtk_text_buffer_get_bounds(buffer, &start, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    FILE *file = fopen(CODE_FILE, "w");
    if (file) {
        fprintf(file, "%s", text);
        fclose(file);
        g_print("Code saved to %s\n", CODE_FILE);
    }
    else {
        g_print("Failed to save code\n");
    }

    g_free(text);
}

void load_file(GtkWidget *widget, gpointer user_data)
{
    EditorComponents *editor_components = (EditorComponents *)user_data;
    GtkTextBuffer *buffer = editor_components->text_buffer;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(widget);

    gchar *filename = gtk_file_chooser_get_filename(chooser);
    if (filename) {
        FILE *file = fopen(filename, "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            long length = ftell(file);
            fseek(file, 0, SEEK_SET);

            char *content = g_malloc(length + 1);
            fread(content, 1, length, file);
            content[length] = '\0';

            gtk_text_buffer_set_text(buffer, content, -1);
            fclose(file);

            g_free(content);
            g_print("Loaded file: %s\n", filename);
        }
        else {
            g_print("Failed to open file: %s\n", filename);
        }
        g_free(filename);
    }
}

void compile_code(GtkWidget *widget, gpointer user_data)
{
    // Access the EditorComponents structure
    EditorComponents *editor_components = (EditorComponents *)user_data;
    GtkWidget *error_label = editor_components->error_label; // Access error_label from the struct

    // Ensure error_label is valid before setting its text
    if (!GTK_IS_LABEL(error_label)) {
        g_print("Error: invalid GtkLabel\n");
        return;
    }

    // Save the current editor content to file before compiling
    save_to_file(editor_components->text_buffer);

    // Compile the saved code
    char command[256];
    snprintf(command, sizeof(command), "./parser_tester %s", CODE_FILE);

    // Run the compilation command
    int status = system(command);

    // Check compilation status and display error or success message
    if (status == 0) {
        gtk_label_set_text(GTK_LABEL(error_label), "Compilation succeeded: No errors detected.");
        g_print("Code compiled successfully to %s\n", OBJ_FILE);
    }
    else {
        FILE *stderr_file = fopen("stderr.log", "r");
        if (stderr_file) {
            fseek(stderr_file, 0, SEEK_END);
            long length = ftell(stderr_file);
            fseek(stderr_file, 0, SEEK_SET);

            char *error_message = g_malloc(length + 1);
            fread(error_message, 1, length, stderr_file);
            error_message[length] = '\0';

            gtk_label_set_text(GTK_LABEL(error_label), error_message);

            fclose(stderr_file);
            g_free(error_message);
        }
        else {
            gtk_label_set_text(GTK_LABEL(error_label), "Unknown error occurred during compilation.");
        }
    }
}

int main(int argc, char *argv[])
{
    GtkWidget *window, *compile_button, *notebook, *file_button, *header_box, *error_label, *error_box, *main_tab, *editor_box, *scroll_window;
    GtkWidget *cursor_position_box, *cursor_position_label;
    EditorComponents editor_components;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Code Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    main_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), main_tab, gtk_label_new("Editor"));

    header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_tab), header_box, FALSE, FALSE, 0);

    file_button = gtk_file_chooser_button_new("Choose File", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_box_pack_start(GTK_BOX(header_box), file_button, FALSE, FALSE, 0);

    compile_button = gtk_button_new_with_label("Compile");
    gtk_box_pack_start(GTK_BOX(header_box), compile_button, FALSE, FALSE, 0);

    scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_tab), scroll_window, TRUE, TRUE, 0);

    editor_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(scroll_window), editor_box);

    editor_components.line_numbers_view = GTK_TEXT_VIEW(gtk_text_view_new());
    GtkTextBuffer *line_buffer = gtk_text_view_get_buffer(editor_components.line_numbers_view);
    gtk_text_view_set_editable(editor_components.line_numbers_view, FALSE);
    gtk_text_view_set_cursor_visible(editor_components.line_numbers_view, FALSE);
    gtk_box_pack_start(GTK_BOX(editor_box), GTK_WIDGET(editor_components.line_numbers_view), FALSE, FALSE, 0);

    editor_components.editor_view = GTK_TEXT_VIEW(gtk_text_view_new());
    editor_components.text_buffer = gtk_text_view_get_buffer(editor_components.editor_view);
    gtk_box_pack_start(GTK_BOX(editor_box), GTK_WIDGET(editor_components.editor_view), TRUE, TRUE, 0);

    error_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_tab), error_box, FALSE, FALSE, 0);

    error_label = gtk_label_new("Status: No actions performed yet.");
    editor_components.error_label = error_label; // Assign the error label to the struct
    gtk_box_pack_start(GTK_BOX(error_box), error_label, TRUE, TRUE, 0);

    cursor_position_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_tab), cursor_position_box, FALSE, FALSE, 0);

    cursor_position_label = gtk_label_new("Line: 1, Col: 1");
    editor_components.cursor_position_label = cursor_position_label; // Assign the cursor position label
    gtk_box_pack_start(GTK_BOX(cursor_position_box), cursor_position_label, FALSE, FALSE, 0);

    g_signal_connect(file_button, "file-set", G_CALLBACK(load_file), &editor_components);
    g_signal_connect(compile_button, "clicked", G_CALLBACK(compile_code), &editor_components);

    // Connect the cursor-position signal to update the position label
    g_signal_connect(editor_components.text_buffer, "changed", G_CALLBACK(update_cursor_position), &editor_components);

    g_signal_connect(editor_components.text_buffer, "changed", G_CALLBACK(update_line_numbers), &editor_components);
    g_signal_connect(gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(editor_components.editor_view)), "value-changed", G_CALLBACK(synchronize_scroll), &editor_components);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
