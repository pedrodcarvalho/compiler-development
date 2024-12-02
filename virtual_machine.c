#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "code_reader.h"

typedef struct {
    GtkWidget *notebook;
    GtkWidget *output_view;
    GtkWidget *execution_mode_combo;
    GtkWidget *execution_button;
} VMComponents;

typedef struct {
    GtkWidget *file_chooser;
    GtkWidget *tree_view;
} FileChooserAndTable;

char *filename;

GPtrArray *parse_stack_log(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        g_print("Failed to open stack.log\n");
        return NULL;
    }

    GPtrArray *stack_states = g_ptr_array_new_with_free_func(g_free);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        g_ptr_array_add(stack_states, g_strdup(line));
    }
    fclose(file);
    return stack_states;
}

void populate_machine_code_table(GtkWidget *widget, gpointer data)
{
    FileChooserAndTable *widgets = (FileChooserAndTable *)data;

    GtkWidget *file_chooser = widgets->file_chooser;
    GtkWidget *tree_view = widgets->tree_view;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    if (!filename) {
        g_print("No file selected\n");
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        g_print("Failed to open %s\n", filename);
        return;
    }

    GtkListStore *store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(tree_view));
    for (GList *iter = columns; iter != NULL; iter = iter->next) {
        gtk_tree_view_remove_column(GTK_TREE_VIEW(tree_view), GTK_TREE_VIEW_COLUMN(iter->data));
    }
    g_list_free(columns);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Line Number", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Command", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Attribute 1", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Attribute 2", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

    char line[256];
    int line_number = 1;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        char command[50], attr1[50], attr2[50];
        int parsed = sscanf(line, "%s %s %s", command, attr1, attr2);

        if (parsed >= 1) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);

            gtk_list_store_set(store, &iter,
                               0, line_number,
                               1, command,
                               2, (parsed >= 2) ? attr1 : "",
                               3, (parsed >= 3) ? attr2 : "",
                               -1);
        }

        line_number++;
    }

    fclose(file);

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void on_stack_state_changed(GtkComboBox *combo, gpointer user_data)
{
    GtkTreeView *stack_view = GTK_TREE_VIEW(user_data);
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_INT);

    int index = gtk_combo_box_get_active(combo);
    if (index >= 0) {
        GPtrArray *stack_states = g_object_get_data(G_OBJECT(combo), "stack_states");
        const char *stack_state = g_ptr_array_index(stack_states, index);

        int address = 0;
        char *token = strtok((char *)stack_state, " ");
        while (token && strcmp(token, "|") != 0) {
            int value = atoi(token);

            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               0, address++,
                               1, value,
                               -1);

            token = strtok(NULL, " ");
        }
    }

    gtk_tree_view_set_model(stack_view, GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void on_combo_destroy(GtkWidget *combo, gpointer user_data)
{
    GPtrArray *stack_states = g_object_get_data(G_OBJECT(combo), "stack_states");
    if (stack_states) {
        g_ptr_array_unref(stack_states);
    }
}

void create_stack_navigation_page(GtkWidget *notebook, GPtrArray *stack_states)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *combo = gtk_combo_box_text_new();
    GtkWidget *stack_view = gtk_tree_view_new();

    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
        "Address", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(stack_view), column);

    column = gtk_tree_view_column_new_with_attributes(
        "Value", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(stack_view), column);

    for (guint i = 0; i < stack_states->len; i++) {
        char instruction_label[32];
        char *token = strtok((char *)g_ptr_array_index(stack_states, i), "|");
        token = strtok(NULL, "|");
        snprintf(instruction_label, sizeof(instruction_label), "Instruction %d: %s", i + 1, token);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), instruction_label);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

    g_object_set_data_full(G_OBJECT(combo), "stack_states", stack_states, (GDestroyNotify)g_ptr_array_unref);
    g_signal_connect(combo, "changed", G_CALLBACK(on_stack_state_changed), stack_view);

    gtk_box_pack_start(GTK_BOX(page), combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(page), stack_view, TRUE, TRUE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, gtk_label_new("Stack Viewer"));
    gtk_widget_show_all(page);
}

void run_code(GtkWidget *widget, gpointer user_data)
{
    read_code(widget, filename);

    GPtrArray *stack_states = parse_stack_log("stack.log");
    if (!stack_states) {
        return;
    }

    create_stack_navigation_page(((VMComponents *)user_data)->notebook, stack_states);

    FILE *file = fopen("output.txt", "r");
    if (file) {
        GtkWidget *text_view = gtk_text_view_new();
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
        GtkWidget *scroll_window = gtk_scrolled_window_new(NULL, NULL);

        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *content = g_malloc(length + 1);
        fread(content, 1, length, file);
        content[length] = '\0';

        gtk_text_buffer_set_text(buffer, content, -1);
        fclose(file);

        GtkWidget *label = gtk_label_new("Output");
        gtk_container_add(GTK_CONTAINER(scroll_window), text_view);
        gtk_widget_show_all(scroll_window);
        gtk_notebook_append_page(GTK_NOTEBOOK(((VMComponents *)user_data)->notebook), scroll_window, label);

        g_free(content);
    }
    else {
        g_print("Failed to open output file\n");
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window, *main_box, *header_box, *machine_code_table, *memory_table, *file_button, *execution_box, *scroll_window;
    VMComponents vm_components;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Virtual Machine");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    vm_components.notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(main_box), vm_components.notebook, TRUE, TRUE, 0);

    GtkWidget *machine_code_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *scrolled_window_machine_code = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled_window_machine_code, -1, 300);

    machine_code_table = gtk_tree_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window_machine_code), machine_code_table);
    gtk_box_pack_start(GTK_BOX(machine_code_page), scrolled_window_machine_code, TRUE, TRUE, 0);
    gtk_notebook_append_page(GTK_NOTEBOOK(vm_components.notebook), machine_code_page, gtk_label_new("Object Code"));

    file_button = gtk_file_chooser_button_new("Choose File", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_box_pack_start(GTK_BOX(main_box), file_button, FALSE, FALSE, 0);

    execution_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    vm_components.execution_button = gtk_button_new_with_label("Run");
    gtk_box_pack_start(GTK_BOX(execution_box), vm_components.execution_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), execution_box, FALSE, FALSE, 0);
    g_signal_connect(vm_components.execution_button, "clicked", G_CALLBACK(run_code), &vm_components);

    FileChooserAndTable *widgets = g_new(FileChooserAndTable, 1);
    widgets->file_chooser = file_button;
    widgets->tree_view = machine_code_table;

    g_signal_connect(file_button, "file-set", G_CALLBACK(populate_machine_code_table), widgets);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
