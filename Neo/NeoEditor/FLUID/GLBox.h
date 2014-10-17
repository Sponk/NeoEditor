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

#include "PostProcessor.h"

class GLBox : public Fl_Gl_Window
{
public:
  GLBox(int X, int Y, int W, int H, const char *L)
      : Fl_Gl_Window(X, Y, W, H, L), maratis_init(false), m_postProcessing(false) {};

  bool maratis_init;
  bool hasPostEffects() { return m_postProcessing; }
  PostProcessor* getPostProcessor() { return &m_postProcessor; }
  void loadPostEffectsFromGame(MGame* game);

  void resize(int x, int y, int w, int h);

  void enablePostEffects() { m_postProcessing = true; }
  void disablePostEffects() { m_postProcessing = false; }

private:
  void draw();
  int handle(int event);

  int mouse_x;
  int mouse_y;

  bool m_postProcessing;
  PostProcessor m_postProcessor;
};

bool getNearestRaytracedDistance(MMesh * mesh, MMatrix4x4 * matrix, const MVector3 & origin, const MVector3 & dest, float * distance, MOEntity * entity = NULL);

#endif
