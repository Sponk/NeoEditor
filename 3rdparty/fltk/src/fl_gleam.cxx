//
// "Gleam" drawing routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//
// These box types provide a sort of Clearlooks Glossy scheme
// for FLTK.
//
// Copyright 2001-2005 by Colin Jones.
//
// Modified 2012 by Edmanuel Torres
// This is a new version of the fl_gleam. The gradients are on the top
// an the bottom, the text area looks like in the classic FLTK way.
//

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <iostream>

using namespace std;

static void gleam_color(Fl_Color c) {
  if (Fl::draw_box_active()) fl_color(c);
  else fl_color(fl_inactive(c));
}

static void shade_rect_top_bottom(int x, int y, int w, int h, Fl_Color fg1, Fl_Color fg2, float th) {
  // Draws the shiny but use maximum limits
  int h_top  = min(h/2,20);
  int h_bottom = min(h/6,20);
  int h_flat = h-(h_top+h_bottom);
  int j = 0;
  float step_size_top = h_top>1?(0.999/(float)(h_top)):1;
  float step_size_bottom = h_bottom>1?(0.999/(float)(h_bottom)):1;
  // This loop generates the gradient at the top of the widget
  for (float k = 1; k >= 0; k -= step_size_top){
    gleam_color(fl_color_average(fl_color_average(fg1, fg2, th), fg1, k));
    fl_line(x, y+j, x+w, y+j);
    j++;
  }
  gleam_color(fg1);
  fl_rectf(x, y+h_top, w+1, h_flat);
  // This loop generates the gradient at the bottom of the widget
  for (float k = 1; k >= 0; k -= step_size_bottom){
    gleam_color(fl_color_average(fg1,fl_color_average(fg1, fg2, th), k));
    fl_line(x, y+j+h_flat-1, x+w, y+j+h_flat-1);
    j++;
  }
}

static void shade_rect_left_right(int x, int y, int w, int h, Fl_Color fg1, Fl_Color fg2, float th) {
  // Draws the shiny useing maximum limits
  int w_left  = min(w/8,10);
  int w_right = min(w/8,10);
  int w_flat = w-(w_left+w_right);
  int j = 0;
  float step_size_left = w_left>1?(0.999/(float)(w_left)):1;
  float step_size_right = w_right>1?(0.999/(float)(w_right)):1;
  // This loop generates the gradient on the left side of the widget
  for (float k = 1; k >= 0; k -= step_size_left){
    gleam_color(fl_color_average(fl_color_average(fg1, fg2, th), fg1, k));
    fl_line(x+j, y, x+j, y+h);
    j++;
  }
  gleam_color(fg1);
  fl_rectf(x+w_left, y, w_flat, h+1);
  // This loop generates the gradient on the right side of the widget
  for (float k = 1; k >= 0; k -= step_size_right){
    gleam_color(fl_color_average(fg1,fl_color_average(fg1, fg2, th), k));
    fl_line(x+j+w_flat-1, y, x+j+w_flat-1, y+h);
    j++;
  }
}

static void shade_rect_top_bottom_up(int x, int y, int w, int h, Fl_Color bc, float th) {
  shade_rect_top_bottom(x,y,w,h,bc,FL_WHITE,th);
}

static void shade_rect_top_bottom_down(int x, int y, int w, int h, Fl_Color bc, float th) {
  shade_rect_top_bottom(x,y,w,h,bc,FL_BLACK,th);
}

static void frame_rect(int x, int y, int w, int h, Fl_Color fg1, Fl_Color fg2) {
  gleam_color(fg1);
  fl_line(x, y+h-1, x, y+1);         //Go from bottom to top left side
  fl_line(x+w, y+h-1, x+w, y+1);     //Go from bottom to top right side
  fl_line(x+1, y, x+w-1, y);         //Go across the top
  fl_line(x+1, y+h, x+w-1, y+h);     //Go across the bottom
  gleam_color(fg2);
  fl_line(x+1, y+h-2, x+1, y+2);     //Go from bottom to top left side
  fl_line(x+w-1, y+h-2, x+w-1, y+2); //Go from bottom to top right side
}

static void frame_rect_up(int x, int y, int w, int h, Fl_Color bc, float th1, float th2) {
  frame_rect(x,y,w,h,fl_color_average(fl_darker(bc), FL_BLACK, th1),fl_color_average(bc, FL_WHITE, th2));
}

static void frame_rect_down(int x, int y, int w, int h, Fl_Color bc, float th1, float th2) {
  frame_rect(x,y,w,h,fl_color_average(bc, FL_WHITE, th2),fl_color_average(FL_BLACK, bc, th1));
}

static void up_frame(int x, int y, int w, int h, Fl_Color c) {
  frame_rect_up(x, y, w-1, h-1, c, 0.15f, 0.15f);
  // frame decoration
  gleam_color(fl_darker(c));
  fl_line(x+2, y+1, x+w-3, y+1);     //Go across the top
  fl_line(x+2, y+h-2, x+w-3, y+h-2); //Go across the bottom
}

static void up_box(int x, int y, int w, int h, Fl_Color c) {
  shade_rect_top_bottom_up(x+2, y+1, w-5, h-3, c, 0.15f);
  frame_rect_up(x, y, w-1, h-1, c, 0.15f, 0.15f);
}

static void thin_up_box(int x, int y, int w, int h, Fl_Color c) {
  shade_rect_top_bottom_up(x+2, y+1, w-5, h-3, c, 0.25f);
  frame_rect_up(x, y, w-1, h-1, c, 0.25f, 0.25f);
}

static void down_frame(int x, int y, int w, int h, Fl_Color c) {
  frame_rect_down(x, y, w-1, h-1, c, 0.75f, 0.35f);
  // frame decoration
  gleam_color(fl_darker(c));
  fl_line(x+2, y+1, x+w-3, y+1);     //Go across the top
  fl_line(x+2, y+h-2, x+w-3, y+h-2); //Go across the bottom
}

static void down_box(int x, int y, int w, int h, Fl_Color c) {
  shade_rect_top_bottom_down(x+1, y+1, w-3, h-3, c, 0.35f);
  frame_rect_down(x, y, w-1, h-1, fl_darker(c), 0.65f, 0.35f);
}

static void thin_down_box(int x, int y, int w, int h, Fl_Color c) {
  shade_rect_top_bottom_down(x+1, y+1, w-3, h-3, c, 0.45f);
  frame_rect_down(x, y, w-1, h-1, fl_darker(c), 0.65f, 0.55f);
}

extern void fl_internal_boxtype(Fl_Boxtype, Fl_Box_Draw_F*);

Fl_Boxtype fl_define_FL_GLEAM_UP_BOX() {
  fl_internal_boxtype(_FL_GLEAM_UP_BOX, up_box);
  fl_internal_boxtype(_FL_GLEAM_DOWN_BOX, down_box);
  fl_internal_boxtype(_FL_GLEAM_UP_FRAME, up_frame);
  fl_internal_boxtype(_FL_GLEAM_DOWN_FRAME, down_frame);
  fl_internal_boxtype(_FL_GLEAM_THIN_UP_BOX, thin_up_box);
  fl_internal_boxtype(_FL_GLEAM_THIN_DOWN_BOX, thin_down_box);
  fl_internal_boxtype(_FL_GLEAM_ROUND_UP_BOX, up_box);
  fl_internal_boxtype(_FL_GLEAM_ROUND_DOWN_BOX, down_box);
  return _FL_GLEAM_UP_BOX;
}
