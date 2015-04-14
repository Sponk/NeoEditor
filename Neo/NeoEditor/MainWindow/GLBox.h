
//========================================================================
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================

#ifndef GLBOX_H
#define GLBOX_H
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.H>
#include <NeoCore.h>
#include <NeoEngine.h>
#include <vector>

#include "../MainWindow/PostProcessor.h"

class GLBox : public Fl_Gl_Window
{
public:
  GLBox(int X, int Y, int W, int H, const char *L)
      : Fl_Gl_Window(X, Y, W, H, L), maratis_init(false), m_postProcessing(false) {};

  bool maratis_init;
  bool hasPostEffects() { return m_postProcessing; }
  Neo::PostProcessor* getPostProcessor() { return &m_postProcessor; }
  void loadPostEffectsFromGame(Neo::NeoGame* game);

  void resize(int x, int y, int w, int h);

  void enablePostEffects() { m_postProcessing = true; }
  void disablePostEffects() { m_postProcessing = false; }

private:
  void draw();
  int handle(int event);

  int mouse_x;
  int mouse_y;

  bool m_postProcessing;
  EditorPostProcessor m_postProcessor;
};

bool getNearestRaytracedDistance(Neo::Mesh * mesh, Neo::Matrix4x4 * matrix, const Neo::Vector3 & origin, const Neo::Vector3 & dest, float * distance, Neo::OEntity * entity = NULL);

#endif
