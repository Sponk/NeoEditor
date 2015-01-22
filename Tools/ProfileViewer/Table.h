#ifndef TABLE_H
#define TABLE_H

#include <FL/Fl_Scroll.H>
#include <FL/Fl_Widget.H>

#define COLS 6
#define ROWS 100

// TODO: Only supports the size of 6x100 !
class RateTable : public Fl_Scroll
{
    void *w[ROWS][COLS];        // widget pointers
public:
    RateTable(int X, int Y, int W, int H, const char*L=0);

    // TODO: Boundary check!
    Fl_Widget* cell(int x, int y) { return (Fl_Widget*) w[x][y]; }
};


#endif
