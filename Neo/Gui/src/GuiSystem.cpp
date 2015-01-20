/*
 * Copyright 2014-2015 (C) Yannick Pflanzer <neo-engine.de>
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
#include <InputField.h>
#include <Render.h>
#include <Label.h>
#include <Sprite.h>
#include <Tile.h>
#include <algorithm>

using namespace Neo;
using namespace Neo::Gui;

GuiSystem::GuiSystem()
{
    m_canvasVector.push_back(Canvas::getInstance());
    m_defaultFont = "fonts/default.ttf";
    m_defaultFontSize = 12.0;
    m_normalBackground = MVector4(0.5,0.5,0.5,1.0);
    m_hoverBackground = MVector4(0.7,0.7,0.7,1.0);
    m_highlightBackground = MVector4(0.6,0.7,0.8,1.0);
    m_enabled = false;
	m_clearScheduled = false;
	m_ids = 0;
}

GuiSystem::~GuiSystem()
{
    Canvas* c;
    for(int i = 0; i < m_canvasVector.size(); i++)
    {
        c = m_canvasVector[i];
        if(c && c != Canvas::getInstance())
            delete c;
    }
}

std::vector<std::string> scriptCallbacks;

void scriptCallback(long int id)
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
    script->callFunction(scriptCallbacks[id].c_str());
}

/**
 * @brief Enables or disable the GUI system.
 * @return Success
 */
int enableGui()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
    if(script->isFunctionOk("enableGui", 1))
        GuiSystem::getInstance()->setEnabled(script->getInteger(0) != 0);

    return 1;
}

int getMainCanvas()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
    script->pushPointer(Canvas::getInstance());
    return 1;
}

int getCanvasClearColor()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("getCanvasClearColor", 1))
        return 0;

    Canvas* c = (Canvas*) script->getPointer(0);

    script->pushFloatArray(c->getClearColor(), 4);
    return 1;
}

int setCanvasClearColor()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("setCanvasClearColor", 2))
        return 0;

    Canvas* c = (Canvas*) script->getPointer(0);
    MVector4 vec;
    script->getFloatArray(1, vec, 4);

    c->setClearColor(vec);

    return 1;
}

int createButton()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("createButton", 6))
        return 0;

    Button* btn = new Button(script->getInteger(0), script->getInteger(1),
                             script->getInteger(2), script->getInteger(3), script->getString(4));

    btn->setCallback((CALLBACK_FUNCTION) scriptCallback);

    scriptCallbacks.push_back(script->getString(5));
    btn->setUserData(scriptCallbacks.size()-1);

    GuiSystem* gui = GuiSystem::getInstance();
    int idx = gui->addWidget(btn);

    script->pushInteger(idx);
    return 1;
}

int createInput()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("createInput", 6))
        return 0;

    InputField* input = new InputField(script->getInteger(0), script->getInteger(1),
                             script->getInteger(2), script->getInteger(3), script->getString(4));

    input->setCallback((CALLBACK_FUNCTION) scriptCallback);

    scriptCallbacks.push_back(script->getString(5));
    input->setUserData(scriptCallbacks.size()-1);

    GuiSystem* gui = GuiSystem::getInstance();
    int idx = gui->addWidget(input);

    script->pushInteger(idx);
    return 1;
}

int createLabel()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("createLabel", 5))
        return 0;

    Label* label = new Label(script->getInteger(0), script->getInteger(1),
                             script->getInteger(2), script->getInteger(3), script->getString(4));

    GuiSystem* gui = GuiSystem::getInstance();
    int idx = gui->addWidget(label);

    script->pushInteger(idx);
    return 1;
}

int createSprite()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("createSprite", 6))
        return 0;

    Sprite* sprite = new Sprite(script->getInteger(0), script->getInteger(1),
                             script->getInteger(2), script->getInteger(3), script->getString(4), script->getString(5));

    GuiSystem* gui = GuiSystem::getInstance();
    int idx = gui->addWidget(sprite);

    script->pushInteger(idx);
    return 1;
}

int addWidgetToCanvas()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("addWidgetToCanvas", 2))
        return 0;

    Canvas* c = (Canvas*) script->getPointer(0);
    c->addWidget(script->getInteger(1));

    return 1;
}

int setLabel()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(script->getArgsNumber() != 2)
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));
    w->setLabel(script->getString(1));

    return 1;
}

int getLabel()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("getLabel", 1))
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));
    script->pushString(w->getLabel());

    return 1;
}

int setDefaultFontSize()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("setDefaultFontSize", 1))
        return 0;

    GuiSystem::getInstance()->setDefaultFontSize(script->getFloat(0));
    return 1;
}

int setWidgetPosition()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("setWidgetPosition", 2))
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

    MVector2 vec;
    script->getFloatArray(1, vec, 2);
    w->setPosition(vec);

    return 1;
}

int setWidgetRotation()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("setWidgetRotation", 2))
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

    w->setRotation(script->getFloat(1));
    return 1;
}

int getWidgetRotation()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("getWidgetRotation", 1))
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));
    script->pushFloat(w->getRotation());

    return 1;
}

int getWidgetPosition()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("getWidgetPosition", 1))
        return 0;

    Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));
    script->pushFloatArray(w->getPosition(), 2);

    return 1;
}

int clearGui()
{
	GuiSystem::getInstance()->scheduleClear();
    return 1;
}

int setNormalBackground()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!script->isFunctionOk("setNormalBackground", 1))
		return 0;

	MVector4 vec;
	script->getFloatArray(0, vec, 4);
	GuiSystem::getInstance()->setNormalBackground(vec);
	return 1;
}

int setHoverBackground()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!script->isFunctionOk("setHoverBackground", 1))
		return 0;

	MVector4 vec;
	script->getFloatArray(0, vec, 4);
	GuiSystem::getInstance()->setHoverBackground(vec);
	return 1;
}

int setHighlightBackground()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!script->isFunctionOk("setHighlightBackground", 1))
		return 0;

	MVector4 vec;
	script->getFloatArray(0, vec, 4);
	GuiSystem::getInstance()->setHoverBackground(vec);
	return 1;
}

int enableCanvasRenderToTexture()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!script->isFunctionOk("enableCanvasRenderToTexture", 2))
		return 0;
	
	Canvas* c = (Canvas*) script->getPointer(0);
	c->enableRenderToTexture(script->getString(1));
	return 1;
}

int disableCanvasRenderToTexture()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if(!script->isFunctionOk("disableCanvasRenderToTexture", 1))
		return 0;
	
	Canvas* c = (Canvas*) script->getPointer(0);
	c->disableRenderToTexture();
	return 1;
}

int createCanvas()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Canvas* c = new Canvas;
	
	GuiSystem::getInstance()->addCanvas(c);
	script->pushPointer(c);
	return 1;
}

int detroyWidget()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("destroyWidget", 1))
        return 0;

    int idx = script->getInteger(0);
    GuiSystem::getInstance()->destroyWidget(idx);
    return 1;
}

int setCanvasLayer()
{
    MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

    if(!script->isFunctionOk("setCanvasLayer", 2))
        return 0;

    Canvas* c = (Canvas*) script->getPointer(0);
    int layer = script->getInteger(1);

    if(c)
        c->setLayer(layer);

    GuiSystem::getInstance()->updateLayers();

    return 1;
}

int loadSpriteSheet()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if(!script->isFunctionOk("loadSpriteSheet", 4))
		return 0;

	const char* path = script->getString(0);
	unsigned int tile_width = script->getInteger(1);
	unsigned int tile_height = script->getInteger(2);
	unsigned int tile_distance = script->getInteger(3);

	TileSheet* sheet = new TileSheet();
	sheet->loadImage(path, tile_width, tile_height, tile_distance);

	script->pushPointer(sheet);

	return 1;
}

int deleteSpriteSheet()
{
	return 0;
}

int createTile()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if(!script->isFunctionOk("createTile", 7))
		return 0;

	Tile* tile = new Tile(script->getInteger(0), script->getInteger(1),
							 script->getInteger(2), script->getInteger(3), script->getString(4), script->getInteger(5), script->getInteger(6));

	GuiSystem* gui = GuiSystem::getInstance();
	int idx = gui->addWidget(tile);

	script->pushInteger(idx);
	return 1;
}

int setTileSpriteSheet()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if(!script->isFunctionOk("setTileSpriteSheet", 2))
		return 0;

	Tile* w = (Tile*) GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if(w && !strcmp(w->getStaticName(), "Tile"))
	{
		w->setTileSheet((TileSheet*) script->getPointer(1));
	}
	else
	{
		MLOG_ERROR("Given object is not of type Tile!");
	}
	return 1;
}

int setTileOffset()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if(!script->isFunctionOk("setTileOffset", 2))
		return 0;

	Tile* w = (Tile*) GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if(!strcmp(w->getStaticName(), "Tile"))
	{
		MVector2 vec;
		script->getFloatArray(1, vec, 2);
		w->setOffset(vec);
	}
	else
	{
		MLOG_ERROR("Given object is not of type Tile!");
	}
	return 1;
}

int setWidgetVisible()
{
	MLOG_INFO("Stub");
	return 1;
}

int isWidgetVisible()
{
	MLOG_INFO("Stub");
	return 1;
}
int setWidgetScale()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setWidgetScale", 2))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	MVector2 vec;
	script->getFloatArray(1, vec, 2);
	w->setScale(vec);

	return 1;
}

int setWidgetFlip()
{
	MScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setWidgetFlip", 2))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	MVector2 vec;
	script->getFloatArray(1, vec, 2);
	w->setFlip(vec);

	return 1;
}

void GuiSystem::setupLuaInterface(MScriptContext* script)
{
	script->addFunction("setWidgetVisible", setWidgetVisible);
	
	script->addFunction("setWidgetScale", setWidgetScale);
	script->addFunction("setWidgetFlip", setWidgetFlip);

    script->addFunction("enableGui", enableGui);
    script->addFunction("getMainCanvas", getMainCanvas);
    script->addFunction("createCanvas", createCanvas);
    script->addFunction("getCanvasClearColor", getCanvasClearColor);
    script->addFunction("setCanvasClearColor", setCanvasClearColor);
    script->addFunction("createButton", createButton);
    script->addFunction("createInput", createInput);
    script->addFunction("createLabel", createLabel);
    script->addFunction("createSprite", createSprite);

    script->addFunction("addWidgetToCanvas", addWidgetToCanvas);
    script->addFunction("destroyWidget", ::detroyWidget);

    script->addFunction("setLabel", setLabel);
    script->addFunction("getLabel", getLabel);

    script->addFunction("setDefaultFontSize", ::setDefaultFontSize);
    script->addFunction("clearGui", clearGui);

	script->addFunction("getWidgetPosition", getWidgetPosition);
	script->addFunction("setWidgetPosition", setWidgetPosition);

	script->addFunction("getWidgetRotation", getWidgetRotation);
	script->addFunction("setWidgetRotation", setWidgetRotation);

    script->addFunction("setHighlightBackground", ::setHighlightBackground);
    script->addFunction("setNormalBackground", ::setNormalBackground);
    script->addFunction("setHoverBackground", ::setHoverBackground);
    
    script->addFunction("enableCanvasRenderToTexture", enableCanvasRenderToTexture);
    script->addFunction("disableCanvasRenderToTexture", disableCanvasRenderToTexture);

    script->addFunction("setCanvasLayer", setCanvasLayer);

	script->addFunction("createTile", createTile);
	script->addFunction("loadSpriteSheet", loadSpriteSheet);
	script->addFunction("setTileSpriteSheet", setTileSpriteSheet);
	script->addFunction("setTileOffset", setTileOffset);

	script->addFunction("setWidgetVisible", setWidgetVisible);
	script->addFunction("isWidgetVisible", isWidgetVisible);
}

void GuiSystem::destroyWidget(int idx)
{
	bool found = false;
	WidgetId id;
	int i;

    map<int,WidgetId>::iterator iter = m_widgets.begin();

    for(; iter != m_widgets.end(); iter++)
	{
        id = iter->second;
		if(id.id == idx)
		{
			found = true;
			break;
		}
	}

	if(!found)
		return;

	delete id.w;
    m_widgets.erase(iter);
}

Widget* GuiSystem::getWidget(unsigned int idx)
{
    return m_widgets[idx].w;
}

void GuiSystem::draw()
{
    unsigned int width, height;

    NeoEngine::getInstance()->getSystemContext()->getScreenSize(&width, &height);
    Render::getInstance()->set2D(width, height);

    if(m_enabled)
    {
        for(int i = 0; i < m_canvasVector.size(); i++)
        {
            m_canvasVector[i]->draw();
        }
    }

    NeoEngine::getInstance()->getRenderingContext()->setScissor(0,0,width,height);
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

	if(m_clearScheduled)
	{
		clear();
		m_clearScheduled = false;
		return;
	}
}

void GuiSystem::clear()
{
    for(int i = 1; i < m_canvasVector.size(); i++)
    {
        delete m_canvasVector[i];
    }

    m_canvasVector.clear();

    // Only leave the main canvas in there
    m_canvasVector.push_back(Canvas::getInstance());
    Canvas::getInstance()->clear();

	for(int i = 0; i < m_widgets.size(); i++)
    {
        delete m_widgets[i].w;
    }

    m_widgets.clear();
	scriptCallbacks.clear();
}

static bool compareCanvasLayer(Canvas* a, Canvas* b)
{
    return (a->getLayer() > b->getLayer());
}

void GuiSystem::addCanvas(Canvas* c)
{
    if(c)
        m_canvasVector.push_back(c);
}

void GuiSystem::updateLayers()
{
    std::sort(m_canvasVector.begin(), m_canvasVector.end(), compareCanvasLayer);
}
