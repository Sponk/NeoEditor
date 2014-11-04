/*
 * Copyright 2014 (C) Yannick Pflanzer <neo-engine.de>
 *
 * This file is part of NeoGui.
 *
 * NeoGui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NeoGui is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with NeoGui.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von NeoGui.
 *
 * NeoGui ist Freie Software: Sie können es unter den Bedingungen
 * der GNU Lesser General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * NeoGui wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU Lesser General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <GuiSystem.h>
#include <Canvas.h>
#include <Button.h>
#include <Input.h>
#include <Render.h>

using namespace Neo;

GuiSystem::GuiSystem()
{
    m_canvasVector.push_back(Canvas::getInstance());
    m_defaultFont = "fonts/default.ttf";
    m_defaultFontSize = 12.0;
    m_normalBackground = MVector4(0.5,0.5,0.5,1.0);
    m_hoverBackground = MVector4(0.7,0.7,0.7,1.0);
    m_highlightBackground = MVector4(0.6,0.7,0.8,1.0);
}

GuiSystem::~GuiSystem()
{
    for(int i = 1; i < m_canvasVector.size(); i++)
    {
        delete m_canvasVector[i];
    }
}

std::vector<std::string> scriptCallbacks;

void scriptCallback(long int id)
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();
    script->callFunction(scriptCallbacks[id].c_str());
}

/**
 * @brief Enables or disable the GUI system.
 * @return Success
 */
int enableGui()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();
    if(script->getArgsNumber() == 1)
        GuiSystem::getInstance()->setEnabled(script->getInteger(0) != 0);

    return 1;
}

int getMainCanvas()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();
    script->pushPointer(Canvas::getInstance());
    return 1;
}

int getCanvasClearColor()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    Canvas* c = (Canvas*) script->getPointer(0);

    script->pushFloatArray(c->getClearColor(), 4);
    return 1;
}

int setCanvasClearColor()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 2)
        return 0;

    Canvas* c = (Canvas*) script->getPointer(0);
    MVector4 vec;
    script->getFloatArray(1, vec, 4);

    c->setClearColor(vec);

    return 1;
}

int createButton()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 6)
        return 0;

    Button* btn = new Button(script->getInteger(0), script->getInteger(1),
                             script->getInteger(2), script->getInteger(3), script->getString(4));

    btn->setCallback(scriptCallback);

    scriptCallbacks.push_back(script->getString(5));
    btn->setUserData(scriptCallbacks.size()-1);

    GuiSystem* gui = GuiSystem::getInstance();
    gui->addWidget(btn);

    script->pushInteger(gui->getNumWidgets() - 1);
    return 1;
}

int createInput()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 6)
        return 0;

    Input* input = new Input(script->getInteger(0), script->getInteger(1),
                             script->getInteger(2), script->getInteger(3), script->getString(4));

    input->setCallback(scriptCallback);

    scriptCallbacks.push_back(script->getString(5));
    input->setUserData(scriptCallbacks.size()-1);

    GuiSystem* gui = GuiSystem::getInstance();
    gui->addWidget(input);

    script->pushInteger(gui->getNumWidgets() - 1);
    return 1;
}


int addWidgetToCanvas()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 2)
        return 0;

    Canvas* c = (Canvas*) script->getPointer(0);
    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(1));

    c->addWidget(w);

    return 1;
}

int setLabel()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 2)
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));
    w->setLabel(script->getString(1));

    return 1;
}

int getLabel()
{
    MScriptContext* script = MEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 1)
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));
    script->pushString(w->getLabel());

    return 1;
}

void GuiSystem::setupLuaInterface(MScriptContext* script)
{
    script->addFunction("enableGui", enableGui);
    script->addFunction("getMainCanvas", getMainCanvas);
    script->addFunction("getCanvasClearColor", getCanvasClearColor);
    script->addFunction("setCanvasClearColor", setCanvasClearColor);
    script->addFunction("createButton", createButton);
    script->addFunction("createInput", createInput);
    script->addFunction("addWidgetToCanvas", addWidgetToCanvas);

    script->addFunction("setLabel", setLabel);
    script->addFunction("getLabel", getLabel);
}

void GuiSystem::draw()
{
    unsigned int width, height;

    MEngine::getInstance()->getSystemContext()->getScreenSize(&width, &height);
    Render::getInstance()->set2D(width, height);

    if(m_enabled)
    {
        for(int i = 0; i < m_canvasVector.size(); i++)
        {
            m_canvasVector[i]->draw();
        }
    }
}

void GuiSystem::update()
{
    if(m_enabled)
    {
        for(int i = 0; i < m_canvasVector.size(); i++)
        {
            m_canvasVector[i]->update();
        }
    }
}
