#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <FL/Fl.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_message.H>
#include <string>

// Maratis includes (for MainWindow.cxx)
#include "../Maratis/Maratis.h"
#include "../MPublish/MPublisher.h"
#include "ini.h"
#include "../MWindow/MWindow.h"

// Is now given via compiler flags
// #define EDITOR_VERSION_STRING "0.1"

typedef struct
{
    std::string path;
    std::string level;
    std::string file_path;
    bool changed;
}open_project_t;

extern open_project_t current_project;
extern Fl_Double_Window* main_window;

extern float rotation_speed;
extern float translation_speed;

extern bool reload_editor;
extern bool update_name;

void edit_text_properties(Fl_Widget*, void*);
void update_behavior_menu();
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);
void update_scene_tree();
void create_behavior_ui(MObject3d* object);
void save_settings();

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

inline int c99_snprintf(char* str, size_t size, const char* format, ...)
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
#define getline c99_getline

inline size_t c99_getline(char** lineptr, size_t* n, FILE* stream) 
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

#endif
