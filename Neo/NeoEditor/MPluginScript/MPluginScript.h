/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Neo
// MPluginScript.h
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

#ifndef MPLUGIN_SCRIPT_H
#define MPLUGIN_SCRIPT_H

#include <MScript/MScript.h>
#include <string>

class MPluginScript : public MScript
{
private:
    void init();

    int id;

    std::string m_name;
    std::string m_author;
    std::string m_contact;
    std::string m_license;
    std::string m_description;

    std::string m_inputUpdate;
    bool m_hasInputMethod;

    static int setName();
    static int setAuthor();
    static int setContact();
    static int setLicense();
    static int setDescription();
    static int enableInputMethod();

public:

    MPluginScript() {}
    ~MPluginScript() {}

    void runScript(const char* filename);
    void callFunction(const char* name);
    bool startCallFunction(const char *name);
    bool endCallFunction(int numArgs);

    std::string getAuthor() { return m_author; }
    std::string getContact() { return m_contact; }
    std::string getLicense() { return m_license; }
    std::string getDescription() { return m_description; }
    std::string getName() { return m_name; }

    bool hasInputMethod() { return m_hasInputMethod; }
    void enableInputMethod(bool value) { m_hasInputMethod = value; }

    std::string getInputUpdate() { return m_inputUpdate; };

    void setId(int id) {this->id = id; }
    int getId() { return id; }
};

#endif
