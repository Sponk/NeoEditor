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

#ifndef NEO_NEOEDITOR_MAINWINDOW_UTILS_H_
#define NEO_NEOEDITOR_MAINWINDOW_UTILS_H_

#include <MCore.h>

/**
 * @brief Calls the native file chooser.
 *
 * This function is compatible with the 'fl_file_chooser' function included in FLTK.
 *
 * @param title The window title.
 * @param files The file types to include.
 * @param dir The directory to start in.
 * @param type The selection type.
 */
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);

/**
 * @brief Puts a "Yes No" question dialog on the screen with the given text.
 *
 * Replaces "fl_ask".
 * @param text The question text.
 * @return The answer that was given
 */
int ask(const char* text);

/**
 * @brief Turns a FLTK color to a vector3.
 *
 * @param c The color.
 * @return The color as a vector3.
 */
MVector3 flColorToVector(int c);

/*
 * Windows helper for POSIX functionality.
 */
#if defined(_MSC_VER)
int c99_snprintf(char* str, size_t size, const char* format, ...);
#define snprintf c99_snprintf
#endif

#if defined(_MSC_VER) || defined(_WIN32)
size_t c99_getline(char** lineptr, size_t* n, FILE* stream);
#define getline c99_getline
#endif

#endif /* NEO_NEOEDITOR_MAINWINDOW_UTILS_H_ */
