#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <FL/Fl.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <string>

typedef struct
{
    std::string path;
    std::string level;
    std::string file_path;
    bool changed;
}open_project_t;

extern open_project_t current_project;

extern float rotation_speed;
extern float translation_speed;

extern bool reload_editor;
extern bool update_name;

void edit_text_properties(Fl_Widget*, void*);
void update_behavior_menu();
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);

#ifdef _MSC_VER

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

#define getline c99_getline

// WARNING: Does not allocate memory as it should when n == 0!
/*inline int c99_getline(char** lineptr, size_t* n, FILE* file)
{
	int count = 0;
	
	if(lineptr == NULL || *lineptr == NULL || n == 0 || *n == 0 || file == NULL)
		return 0;
	
	char* line = *lineptr;
	while(count < *n && (line[count] = getc(file)) != '\n'))
	{
		if(line[count] == EOF)
		{
			line[count] = '\0';
		}
		
		count++;
	}

	return count;
}*/

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

    c = fgetc(stream);

    if (c == EOF) 
	{
    	return -1;
    }
    
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
    
	while(c != EOF) 
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
