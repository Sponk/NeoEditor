// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef NeoStore_h
#define NeoStore_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>

class NeoStore {
public:
  Fl_Double_Window* create_window();
  Fl_Browser *available_packages;
  Fl_Help_View *package_description;
  Fl_Help_View *package_detail;
  Fl_Button *install_button;
  Fl_Button *remove_button;
  Fl_Menu_Bar *menu_bar;
  static Fl_Menu_Item menu_menu_bar[];
  static void package_selected(Fl_Browser* browser, NeoStore* self);
  static void install_package(Fl_Button* btn, NeoStore* self);
  static void remove_package(Fl_Button* btn, NeoStore* self);
  static void reload_repositories(Fl_Menu* menu, NeoStore* self);
};
#include <FL/Fl_Progress.H>
#include <FL/Fl_Box.H>

class InstallationDlg {
public:
  Fl_Window* fl_win; 
private:
  unsigned int max_size; 
  char informationLabel[100]; 
  char progressLabel[100]; 
public:
  Fl_Double_Window* create_window();
  Fl_Progress *progress_bar;
  Fl_Box *information_label;
  void close();
  void setMaxSize(unsigned int sz);
  void update(unsigned int curSz);
};
#endif
