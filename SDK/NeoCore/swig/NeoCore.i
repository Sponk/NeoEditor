%{
#include <NeoCore.h>
using namespace Neo;
%}

%include <windows.i>
%include <cpointer.i>
%include <std_string.i>

#define NEO_CORE_EXPORT

// Turn off StdFile since it produces errors
#define __STD_FILE_H
#define __FILE_H
#define __THREAD_H
#define __UTILS_H

%include <NeoCore.h>

//  Some pointer stuff
%pointer_class(int, intp);
%pointer_class(unsigned int, uintp);
%pointer_class(float, floatp);
%pointer_class(double, doublep);

%pointer_cast(void*, Neo::String*, Voidp2String);
%pointer_cast(void*, float*, Voidp2Float);
//%pointer_functions(float*, floatp)

%pointer_class(Neo::String, Stringp);
%pointer_class(Neo::Vector3, Vector3p);


