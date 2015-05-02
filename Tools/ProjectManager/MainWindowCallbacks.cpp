#include "MainWindow.h"

#include <FL/Fl.H>
#include <FL/fl_message.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <NeoCore.h>

#include <iostream>
#include <fstream>
#include <algorithm>


#ifdef WIN32
#include <windows.h>
#endif

using namespace std;

extern string currentDirectory;

void executeBlocking(const char* cmd)
{
	// FIXME: Don't use system!!!
#ifndef WIN32
    system(cmd);
#else
    //ShellExecute(NULL, "open",
    //			(currentDirectory + "\\NeoStore.exe").c_str(),
    //			NULL,NULL, 1);

    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = cmd;
    ShExecInfo.lpParameters = "";
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;

    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
#endif
}

// FLTK helper
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type)
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

void MainWindow::open_neo_store(Fl_Button*, MainWindow* dlg)
{
#ifndef WIN32
	std::string path = currentDirectory + "/NeoStore";
#else
    std::string path = currentDirectory + "\\NeoStore.exe";
#endif

	executeBlocking(path.c_str());
    dlg->update_package_list();
}

void MainWindow::update_package_list()
{
    // This file needs to be the same as in the NeoStore!
    std::string confPath;
#ifndef WIN32
    confPath = getenv("HOME");
    confPath += "/.neo-store/";
#else
    confPath = getenv("APPDATA");
    confPath += "\\.neo-store\\";
#endif

    confPath += "installed.txt";

    std::ifstream in(confPath.c_str());

    if(!in)
        return;

    package_browser->clear();

    std::string line;
    while(!in.eof())
    {
        line = "";
        in >> line;

        if(!line.empty())
            package_browser->add(line.c_str());
    }

    in.close();
}

void MainWindow::import_project(Fl_Button* btn, MainWindow* dlg)
{
    const char* projectFile = fl_native_file_chooser("Choose a project", "*.mproj",
                                                     NULL, Fl_Native_File_Chooser::BROWSE_FILE);

    if(projectFile == NULL)
        return;

    dlg->addProject(projectFile);
    dlg->saveProjectList();
}

void MainWindow::install_package(Fl_Button* btn, MainWindow* dlg)
{
    int value = dlg->project_browser->value();

    if(value <= 0)
    {
        fl_message("You need to select a project!");
        return;
    }

    std::string name = dlg->project_browser->text(value);

    if(name.empty())
    {
        fl_message("You need to select a project!");
        return;
    }

    std::string path = dlg->getProjectPath(name.c_str());

    if(path.empty())
        return;

    // ATTENTION: Needs to be the same as in NeoStore!
    std::string package;

#ifndef WIN32
    package = getenv("HOME");
    package += "/.neo-store/";
#else
    package = getenv("APPDATA");
    package += "\\.neo-store\\";
#endif

    value = dlg->package_browser->value();

    if(value <= 0)
    {
        fl_message("You need to select a package!");
        return;
    }

    std::string pkgName = dlg->package_browser->text(value);
    package += pkgName.erase(pkgName.find_last_of("-"));

#ifdef WIN32
    std::replace(package.begin(), package.end(), '\\', '/');
    std::replace(path.begin(), path.end(), '\\', '/');
#endif

    fl_message("Copying %s to %s", package.c_str(), path.c_str());
    if(!copyDirectory(package.c_str(), path.c_str()))
    {
        fl_message("Could not install package!");
    }
    else
    {
        fl_message("Package installation was successful.");
    }
}

const char* MainWindow::getProjectPath(const char* name)
{
    return m_projects[std::string(name)].c_str();
}

void MainWindow::addProject(const char* name, const char* path)
{
    m_projects[std::string(name)] = path;
}

void MainWindow::loadProjectList()
{
    std::string path;
#ifndef WIN32
    path = getenv("HOME");
    path += "/.neo-project-manager/";
#else
    path = getenv("APPDATA");
    path += "\\.neo-project-manager\\";
#endif

    if(!isFileExist(path.c_str()))
        createDirectory(path.c_str(), true);

    path += "projects.txt";

    std::ifstream in(path.c_str());

    if(!in)
        return;

    std::string line;
    while(!in.eof())
    {
        line = "";
        in >> line;

        if(!line.empty())
            addProject(line.c_str());
    }

    in.close();
}

void MainWindow::saveProjectList()
{
    std::string path;
#ifndef WIN32
    path = getenv("HOME");
    path += "/.neo-project-manager/";
#else
    path = getenv("APPDATA");
    path += "\\.neo-project-manager\\";
#endif

    if(!isFileExist(path.c_str()))
        createDirectory(path.c_str(), true);

    path += "projects.txt";

    std::ofstream out(path.c_str());

    if(!out)
        return;

    char sep;

#ifndef WIN32
    sep = '/';
#else
    sep = '\\';
#endif

	for (std::map<std::string, std::string>::iterator iter = m_projects.begin();
		 iter != m_projects.end(); ++iter)
	{
		out << iter->second << sep << iter->first << endl;
	}

	out.close();
}

void MainWindow::addProject(const char* filepath)
{
    std::string path = filepath;

    int idx;
#ifndef WIN32
    idx = path.find_last_of("/");
#else
    idx = path.find_last_of("\\");
#endif

    if(idx == -1)
        return;

    std::string name = path.substr(idx+1);
    path = path.erase(idx);

    project_browser->add(name.c_str());
    addProject(name.c_str(), path.c_str());
}

void MainWindow::copy_lua_sdk(Fl_Button*, MainWindow* dlg)
{
    int value = dlg->project_browser->value();

    if(value <= 0)
    {
        fl_message("You need to select a project!");
        return;
    }

    std::string name = dlg->project_browser->text(value);

    if(name.empty())
    {
        fl_message("You need to select a project!");
        return;
    }

    std::string path = dlg->getProjectPath(name.c_str());

    if(path.empty())
        return;

    char src[255];
    char dir[255];
    getGlobalFilename(src, currentDirectory.c_str(), "LuaApi");
    getGlobalFilename(dir, path.c_str(), "scripts/SDK");

    // fl_message("Copy %s to %s", src, dir);

    if(!copyDirectory(src, dir))
    {
        fl_message("Could not copy the Lua SDK!");
    }
}

void MainWindow::copy_cpp_sdk(Fl_Button*, MainWindow* dlg)
{
    int value = dlg->project_browser->value();

    if(value <= 0)
    {
        fl_message("You need to select a project!");
        return;
    }

    std::string name = dlg->project_browser->text(value);

    if(name.empty())
    {
        fl_message("You need to select a project!");
        return;
    }

    std::string path = dlg->getProjectPath(name.c_str());

    if(path.empty())
        return;

    char src[255];
    char dir[255];
    getGlobalFilename(src, currentDirectory.c_str(), "SDK");
    getGlobalFilename(dir, path.c_str(), "SDK");

    // fl_message("Copy %s to %s", src, dir);

    if(!copyDirectory(src, dir))
    {
        fl_message("Could not copy the C++ SDK!");
    }
}

void MainWindow::create_project(Fl_Button*, MainWindow* dlg)
{
#ifndef WIN32
	std::string path = currentDirectory + "/ProjectWizard";
#else
	std::string path = currentDirectory + "\\ProjectWizard";
#endif

	executeBlocking(path.c_str());
}
