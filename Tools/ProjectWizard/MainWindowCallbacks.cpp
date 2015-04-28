#include "MainWindow.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <NeoCore.h>
#include <string>

using namespace std;

#ifndef WIN32
#define SEP "/"
#define HOMEVAR "HOME"
#else
#define SEP "\\"
#define HOMEVAR "APPDATA"
#endif

const char* fl_native_file_chooser(const char* title, const char* files,
								   const char* dir, int type)
{
	Fl_Native_File_Chooser* dlg = new Fl_Native_File_Chooser();
	dlg->title(title);
	dlg->filter(files);
	dlg->type(type);
	dlg->directory(dir);

	if (dlg->show() == 0)
		return dlg->filename();

	return NULL;
}

void MainWindow::next_page(Fl_Button* b, MainWindow* w)
{
	w->wizard->next();
	w->prevButton->activate();

	if (w->readyForFinish)
	{
		if (generateProjectStructure(w->project_directory_edit->value(),
									 w->project_name_edit->value(),
									 w->game_type_choice->value() == 0) != NULL)
			// Is this clean?
			exit(0);
	}

	if (w->currentPage == w->wizard->children() - 2)
	{
		// FIXME: Use tr()!
		w->nextButton->label("Finish");
		w->readyForFinish = true;
	}
	else
		w->currentPage++;
}

void MainWindow::prev_page(Fl_Button* b, MainWindow* w)
{
	w->wizard->prev();
	w->nextButton->label("Next");

	w->readyForFinish = false;
	
	if(w->currentPage <= 0)
		w->prevButton->deactivate();
	else
		w->currentPage--;
}

void MainWindow::find_project_dir(Fl_Button* b, MainWindow* w)
{
	const char* dir = fl_native_file_chooser(
		"Select project directory", NULL,
		getenv(HOMEVAR), Fl_Native_File_Chooser::BROWSE_DIRECTORY);

	if(dir)
		w->project_directory_edit->value(dir);
}

const char* MainWindow::generateProjectStructure(const char* path,
												 const char* name, bool scene)
{
	std::string p = path;
	printf("%s\n", (p + SEP + "assets").c_str());
	if (!isFileExist((p + SEP + "assets").c_str()) &&
		!createDirectory((p + SEP + "assets").c_str(), true))
	{
		fl_alert("Could not create project directory!");
		removeDirectory(path);
		return NULL;
	}

	std::string levelFile;
	if (scene)
	{
		levelFile = std::string("assets/") + name + ".level";
		string levelSystemPath = levelFile;

#ifdef WIN32
		replace(levelSystemPath.begin(), levelSystemPath.end(), '/', '\\');
#endif

		FILE* file = fopen(levelSystemPath.c_str(), "w");
		if (!file)
		{
			fl_alert("Could not create level!");
			removeDirectory(path);
			return NULL;
		}

		fprintf(file, "<Neo version=\"0.5\">\n");
		fprintf(file, "<Level><properties currentScene=\"0\"/>");
		fprintf(file, "<Scene name=\"Scene-1\">");
		fprintf(file, "<script file=\"assets/main.lua\"");
		fprintf(file,
				"<properties data=\"Static\" gravity=\"0.000000 0.000000 "
				"-0.981000\" "
				"ambientLight=\"0.0 0.0 0.0\"/></Scene></Level></Neo>");

		fclose(file);
	}

	std::string project_path = p + "/" + name + ".mproj";

#ifdef WIN32
	replace(project_path.begin(), project_path.end(), '/', '\\');
#endif

	FILE* file = fopen(project_path.c_str(), "w");
	if (!file)
	{
		fl_message("Could not create project file!");
		removeDirectory(path);
		return NULL;
	}

	fprintf(file, "<Neo version=\"0.5\">\n");
	fprintf(file, "<Project><renderer name=\"StandardRenderer\"/>"
				  "<start file=\"%s\"/></Project></Neo>",
			levelFile.c_str());

	fclose(file);

	return project_path.c_str();
}
