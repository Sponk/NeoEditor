
#include <gtk/gtk.h>

static int initialized = 0;

static void updateGtk()
{
	while (gtk_events_pending())
		gtk_main_iteration();
}

void initToolkit()
{
	gtk_init(NULL, NULL);

}

int getOpenFilename(lua_State* L)
{
	const char* startdir = lua_tostring(L, 1);

	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;

	int retval = 0;

	dialog = gtk_file_chooser_dialog_new("Open File",
										 NULL,
										 action,
										 GTK_STOCK_CANCEL,
										 GTK_RESPONSE_CANCEL,
										 GTK_STOCK_OPEN,
										 GTK_RESPONSE_ACCEPT,
										 NULL);

	gtk_file_chooser_set_current_folder(dialog, startdir);


	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);

		lua_pushstring(L, filename);

		g_free (filename);
		retval = 1;
	}
	else
		lua_pushstring(L, NULL);

	gtk_widget_destroy (dialog);
	updateGtk();

	return retval;
}

int getSaveFilename(lua_State* L)
{
	const char* startdir = lua_tostring(L, 1);

	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;

	int retval = 0;

	dialog = gtk_file_chooser_dialog_new("Save File",
										 NULL,
										 action,
										 GTK_STOCK_CANCEL,
										 GTK_RESPONSE_CANCEL,
										 GTK_STOCK_SAVE,
										 GTK_RESPONSE_ACCEPT,
										 NULL);

	gtk_file_chooser_set_current_folder(dialog, startdir);


	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		filename = gtk_file_chooser_get_filename (chooser);

		lua_pushstring(L, filename);

		g_free (filename);
		retval = 1;
	}
	else
		lua_pushstring(L, NULL);

	gtk_widget_destroy (dialog);
	updateGtk();

	return retval;
}
