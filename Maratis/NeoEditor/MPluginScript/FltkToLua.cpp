/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Neo
// FltkToLua.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2014 Yannick Pflanzer <www.scary-squid.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <MScript/MScript.h>
#include <FL/Fl.H>
#include <FL/fl_message.H>

// Declared in Callbacks.h and implemented in Callbacks.cpp
const char* fl_native_file_chooser(const char* title, const char* files, const char* dir, int type);

int messagebox()
{
    MScript* script = (MScript*) MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    fl_message(script->getString(0));
    return 1;
}

void createFltkLuaBindings(MScript* script)
{
    if(!script)
        return;

    script->addFunction("messagebox", messagebox);
}
