#ifndef GLBOX_H
#define GLBOX_H
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>
#include <MCore.h>
#include <MEngine.h>
#include <vector>

class GLBox : public Fl_Gl_Window
{
public:
  GLBox(int X, int Y, int W, int H, const char *L)
      : Fl_Gl_Window(X, Y, W, H, L), maratis_init(false) {};

  bool maratis_init;

private:
  void draw();
  int handle(int event);

  int mouse_x;
  int mouse_y;
};

bool getNearestRaytracedDistance(MMesh * mesh, MMatrix4x4 * matrix, const MVector3 & origin, const MVector3 & dest, float * distance, MOEntity * entity = NULL);

#endif
