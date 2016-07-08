%module NeoEditor
%{
#include <LuaInterface.h>
#include <EditorGame.h>

using namespace Neo2D;
using namespace Gui;
%}

%include <std_shared_ptr.i>

%define const
%enddef

 %include "src/LuaInterface.h"
%include "src/EditorGame.h"
