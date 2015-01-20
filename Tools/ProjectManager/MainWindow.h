// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef MainWindow_h
#define MainWindow_h
#include <FL/Fl.H>
#include <string>
#include <map>
#include <FL/Fl_Chart.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Check_Button.H>

class MainWindow {
  std::map<std::string,std::string> m_projects; 
public:
  Fl_Double_Window* create_window();
  Fl_Browser *project_browser;
  Fl_Browser *package_browser;
  static void open_neo_store(Fl_Button*, MainWindow* dlg);
  void update_package_list();
  static void import_project(Fl_Button* btn, MainWindow* dlg);
  static void install_package(Fl_Button* btn, MainWindow* dlg);
  const char* getProjectPath(const char* name);
  void addProject(const char* name, const char* path);
  void loadProjectList();
  void saveProjectList();
  void addProject(const char* filepath);
  static void copy_lua_sdk(Fl_Button*, MainWindow* dlg);
  static void copy_cpp_sdk(Fl_Button*, MainWindow* dlg);
  static void create_project(Fl_Button*, MainWindow* dlg);
  Fl_Double_Window* createNewProjectDlg();
  Fl_Input *project_name_edit;
  Fl_Input *project_directory_edit;
  Fl_Check_Button *install_lua_check;
  Fl_Check_Button *create_initial_scene_check;
  static void find_project_directory(Fl_Button*, MainWindow* dlg);
  static void cancel_np_dialog(Fl_Button* btn, void* dlg);
  const char* generateProjectStructure(const char* path, const char* name, bool lua, bool scene);
  static void generate_project(Fl_Button* btn, MainWindow* dlg);
};
#endif
