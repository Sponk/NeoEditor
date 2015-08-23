--- filedlg.lua - Contains functions for opening native file open and save
-- dialogs for each operating system

local ffi = require("ffi")

local libgtk = ffi.load("libgtk-x11-2.0.so.0")

ffi.cdef[[
// Taken from the FLTK native file chooser
/* --------------------- Type definitions from GLIB and GTK --------------------- */
/* all of this is from the public gnome API, so unlikely to change */
/*#ifndef	FALSE
#define	FALSE	(0)
#endif
#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif*/
typedef void* gpointer;
typedef int    gint;
typedef unsigned int    guint;
typedef unsigned long   gulong;
typedef gint   gboolean;
typedef char   gchar;
typedef struct _GSList GSList;
struct _GSList
{
  gpointer data;
  GSList *next;
};
//#define  g_slist_next(slist)	         ((slist) ? (((GSList *)(slist))->next) : NULL)
typedef struct _GtkWidget      GtkWidget;
typedef struct _GtkFileChooser      GtkFileChooser;
typedef struct _GtkDialog        GtkDialog;
typedef struct _GtkWindow          GtkWindow;
typedef struct _GdkDrawable           GdkWindow;
typedef struct _GtkFileFilter     GtkFileFilter;
typedef struct _GtkToggleButton       GtkToggleButton;

/*typedef enum {
  GTK_FILE_FILTER_FILENAME     = 1 << 0,
  GTK_FILE_FILTER_URI          = 1 << 1,
  GTK_FILE_FILTER_DISPLAY_NAME = 1 << 2,
  GTK_FILE_FILTER_MIME_TYPE    = 1 << 3
} GtkFileFilterFlags;*/

 //int GTK_FILE_FILTER_FILENAME     = 1;
 //int GTK_FILE_FILTER_URI          = 2;
 //int GTK_FILE_FILTER_DISPLAY_NAME = 4;
 //int GTK_FILE_FILTER_MIME_TYPE    = 8;

typedef int GtkFileFilterFlags;


struct _GtkFileFilterInfo
{
  GtkFileFilterFlags contains;

  const gchar *filename;
  const gchar *uri;
  const gchar *display_name;
  const gchar *mime_type;
};

//typedef struct _GtkFileFilterInfo GtkFileFilterInfo;
//typedef gboolean (*GtkFileFilterFunc) (const GtkFileFilterInfo *filter_info, gpointer data);
//typedef void (*GDestroyNotify)(gpointer data);

typedef enum
{
  GTK_FILE_CHOOSER_ACTION_OPEN,
  GTK_FILE_CHOOSER_ACTION_SAVE,
  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
  GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER
} GtkFileChooserAction;

//#define GTK_STOCK_CANCEL           "gtk-cancel"
//#define GTK_STOCK_SAVE             "gtk-save"
//#define GTK_STOCK_OPEN             "gtk-open"

/*const int   GTK_RESPONSE_NONE = -1;
const int   GTK_RESPONSE_ACCEPT = -3;
const int   GTK_RESPONSE_DELETE_EVENT = -4;
const int   GTK_RESPONSE_CANCEL = -6;*/

//typedef void  (*GCallback)(void);
//#define	G_CALLBACK(f)			 ((GCallback) (f))
typedef int GConnectFlags;
typedef struct _GClosure		 GClosure;
typedef void  (*GClosureNotify)(gpointer data, GClosure	*closure);


GtkWidget* gtk_file_chooser_dialog_new (const char *title,
			     GtkWindow *parent,
			     int action,
			     const char *first_button_text,
			     ...);

int gtk_dialog_run (GtkDialog* dialog);
void gtk_widget_show_all (GtkWidget *widget);
void gtk_init(int argc, char* argv[]);

int gtk_events_pending();
void gtk_main_iteration();

]]

GTK_FILE_FILTER_FILENAME     = 1;
GTK_FILE_FILTER_URI          = 2;
GTK_FILE_FILTER_DISPLAY_NAME = 4;
GTK_FILE_FILTER_MIME_TYPE    = 8;

GTK_STOCK_CANCEL = "gtk-cancel"
GTK_STOCK_SAVE = "gtk-save"
GTK_STOCK_OPEN = "gtk-open"

GTK_RESPONSE_NONE = -1;
GTK_RESPONSE_ACCEPT = -3;
GTK_RESPONSE_DELETE_EVENT = -4;
GTK_RESPONSE_CANCEL = -6;

function getOpenFilename(start)

	libgtk.gtk_init(0, nil)

	--dialog = gtk_file_chooser_dialog_new("Open File",
	--									  NULL,
	--									  action,
	--									  GTK_STOCK_CANCEL,
	--									  GTK_RESPONSE_CANCEL,
	--									  GTK_STOCK_OPEN,
	--									  GTK_RESPONSE_ACCEPT,
	--									  NULL);

	local wdg = libgtk.gtk_file_chooser_dialog_new("Open File",
	nil,
	0,
	GTK_STOCK_CANCEL,
	GTK_RESPONSE_CANCEL,
	GTK_STOCK_OPEN,
	GTK_RESPONSE_ACCEPT,
	nil);

	local dlg = ffi.new("GtkDialog*", ffi.cast("GtkDialog*", wdg))
	print(dlg, wdg)

	libgtk.gtk_dialog_run(dlg)

	while libgtk.gtk_events_pending() do
		libgtk.gtk_main_iteration()
	end

end

--getOpenFilename("/home")

local LuaCommonDlg = require("LuaCommonDlg")

infoLog(LuaCommonDlg.getOpenFilename("/home"))
infoLog(LuaCommonDlg.getSaveFilename("/home"))

