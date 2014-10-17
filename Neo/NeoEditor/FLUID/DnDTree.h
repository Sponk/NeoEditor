#ifndef DND_TREE_H
#define DND_TREE_H

#include <FL/Fl_Tree.H>

class DnDTree : public Fl_Tree
{
public:
    DnDTree(int X, int Y, int W, int H, const char *L = 0)
        : Fl_Tree(X, Y, W, H, L) {};
    int handle(int e);
};

#endif
