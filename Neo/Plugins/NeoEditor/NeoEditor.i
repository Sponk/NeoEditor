%module NeoEditor
%{
#include <LuaInterface.h>
#include <EditorGame.h>

using namespace Neo2D;
using namespace Gui;
%}

%include "src/LuaInterface.h"
%include "src/EditorGame.h"
