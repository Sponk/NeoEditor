#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Float_Input.H>
#include "Table.h"

RateTable::RateTable(int X, int Y, int W, int H, const char*L) : Fl_Scroll(X,Y,W,H,L)
{
        static const char *header[COLS] =
        {
            "Procedure", "Calls", "Self time", "Time %", "Total time", "Total %"
        };

        int cellw = 80;
        int curw = cellw;
        int cellh = 25;
        int xx = X, yy = Y;
        Fl_Tile *tile = new Fl_Tile(X,Y,cellw*COLS,cellh*ROWS);
        // Create widgets
        for ( int r=0; r<ROWS; r++ )
        {
            for ( int c=0; c<COLS; c++ )
            {
                if(c == 0)
                    curw = 2*cellw;
                else
                    curw = cellw;

                if ( r==0 )
                {
                    Fl_Box *box = new Fl_Box(xx,yy,curw,cellh,header[c]);
                    box->box(FL_BORDER_BOX);
                    w[r][c] = (void*)box;
                }
                else// if ( c==0 )
                {
                    Fl_Input *in = new Fl_Input(xx,yy,curw,cellh);
                    in->box(FL_BORDER_BOX);
                    in->value("");
                    w[r][c] = (void*)in;
                }
                /*else
                {
                    Fl_Float_Input *in = new Fl_Input(xx,yy,cellw,cellh);
                    in->box(FL_BORDER_BOX);
                    in->value("0.00");
                    w[r][c] = (void*)in;
                }*/
                xx += curw;
            }
            xx = X;
            yy += cellh;
        }
        tile->end();
        end();
    }
