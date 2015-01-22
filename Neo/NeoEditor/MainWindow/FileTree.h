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

#ifndef FILE_TREE_H
#define FILE_TREE_H

#include <FL/Fl_File_Browser.H>
#include <string>

class FileTree : public Fl_File_Browser
{
	std::string m_currentPath;
public:
    FileTree(int X, int Y, int W, int H, const char *L = 0)
        : Fl_File_Browser(X, Y, W, H, L) {};
    int handle(int e);

    const char* currentPath() { return m_currentPath.c_str(); }
    int load(const char* directory, Fl_File_Sort_F * sort = fl_numericsort);
};

#endif
