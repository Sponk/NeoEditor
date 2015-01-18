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

#include "Utils.h"
#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>

const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type)
{
    Fl_Native_File_Chooser* dlg = new Fl_Native_File_Chooser();
    dlg->title(title);
    dlg->filter(files);
    dlg->type(type);
    dlg->directory(dir);

    if(dlg->show() == 0)
        return dlg->filename();

    return NULL;
}

int ask(const char* text)
{
    return fl_choice("%s", "No", "Yes", NULL, text);
}

MVector3 flColorToVector(int c)
{
    char unsigned bytes[4];
    bytes[0] = (c >> 24) & 0xFF;
    bytes[1] = (c >> 16) & 0xFF;
    bytes[2] = (c >> 8) & 0xFF;
    bytes[3] = c & 0xFF;

    return MVector3(static_cast<float>(bytes[0])/255.0f, static_cast<float>(bytes[1])/255.0f, static_cast<float>(bytes[2])/255.0f);
}

/*
 * Windows helper for POSIX functionality.
 */

#if defined(_MSC_VER)

#include <cstdio>
#include <cstdlib>

#define snprintf c99_snprintf

inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

int c99_snprintf(char* str, size_t size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}
#endif

#if defined(_MSC_VER) || defined(_WIN32)
size_t c99_getline(char** lineptr, size_t* n, FILE* stream)
{
    char* bufptr = NULL;
    char* p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL)
	{
    	return -1;
    }

	if (stream == NULL)
	{
    	return -1;
    }

	if (n == NULL)
	{
    	return -1;
    }

	bufptr = *lineptr;
    size = *n;

    if (feof(stream))
	{
    	return -1;
    }

	c = fgetc(stream);

	if (bufptr == NULL)
	{
    	bufptr = (char*) malloc(128);
    	if (bufptr == NULL)
		{
    		return -1;
    	}
    	size = 128;
    }

	p = bufptr;

	while(!feof(stream))
	{
    	if ((p - bufptr) > (size - 1))
		{
    		size = size + 128;
    		bufptr = (char*) realloc(bufptr, size);

			if (bufptr == NULL)
			{
    			return -1;
    		}
    	}

    	*p++ = c;

		if (c == '\n')
		{
    		break;
    	}

		c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}


#endif // _MSC_VER
