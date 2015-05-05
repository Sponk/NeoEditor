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
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit
 *diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#include <GuiSystem.h>
#include <Canvas.h>
#include <Button.h>
#include <ThemedButton.h>
#include <InputField.h>
#include <Render.h>
#include <Label.h>
#include <Sprite.h>
#include <Tile.h>
#include <LuaScript.h>
#include <algorithm>

using namespace Neo;
using namespace Neo::Gui;

GuiSystem::GuiSystem()
{
	m_canvasVector.push_back(Canvas::getInstance());
	m_defaultFont = "assets/default.ttf";

	setThemeDirectory("assets");
	
	m_defaultFontSize = 12.0;
	m_normalBackground = Vector4(0.5, 0.5, 0.5, 1.0);
	m_hoverBackground = Vector4(0.7, 0.7, 0.7, 1.0);
	m_highlightBackground = Vector4(0.6, 0.7, 0.8, 1.0);
	m_enabled = true;
	m_clearScheduled = false;
	m_ids = 0;
}

GuiSystem::~GuiSystem()
{
	Canvas* c;
	for (int i = 0; i < m_canvasVector.size(); i++)
	{
		c = m_canvasVector[i];
		if (c && c != Canvas::getInstance())
			delete c;
	}
}

std::vector<std::string> scriptCallbacks;

void scriptCallback(long int id)
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	script->callFunction(scriptCallbacks[id].c_str());
}

/**
 * @brief Enables or disable the GUI system.
 * @return Success
 */
int enableGui()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if (script->isFunctionOk("enableGui", 1))
		GuiSystem::getInstance()->setEnabled(script->getInteger(0) != 0);

	return 1;
}

int getMainCanvas()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	script->pushPointer(Canvas::getInstance());
	return 1;
}

int getCanvasClearColor()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("getCanvasClearColor", 1))
		return 0;

	Canvas* c = (Canvas*)script->getPointer(0);

	if (c)
		script->pushFloatArray(c->getClearColor(), 4);
	return 1;
}

int setCanvasClearColor()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setCanvasClearColor", 2))
		return 0;

	Canvas* c = (Canvas*)script->getPointer(0);
	Vector4 vec;
	script->getFloatArray(1, vec, 4);

	if (c)
		c->setClearColor(vec);

	return 1;
}

int createButton()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("createButton", 6))
		return 0;

	Button* btn = new Button(script->getInteger(0), script->getInteger(1),
							 script->getInteger(2), script->getInteger(3),
							 script->getString(4));

	btn->setCallback((CALLBACK_FUNCTION)scriptCallback);

	scriptCallbacks.push_back(script->getString(5));
	btn->setUserData(scriptCallbacks.size() - 1);

	GuiSystem* gui = GuiSystem::getInstance();
	int idx = gui->addWidget(btn);

	script->pushInteger(idx);
	return 1;
}

int createThemedButton()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("createThemedButton", 6))
		return 0;

	ThemedButton* btn = new ThemedButton(script->getInteger(0), script->getInteger(1),
							 script->getInteger(2), script->getInteger(3),
							 script->getString(4));

	btn->setCallback((CALLBACK_FUNCTION)scriptCallback);

	scriptCallbacks.push_back(script->getString(5));
	btn->setUserData(scriptCallbacks.size() - 1);

	GuiSystem* gui = GuiSystem::getInstance();
	int idx = gui->addWidget(btn);

	script->pushInteger(idx);
	return 1;
}

int createInput()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("createInput", 6))
		return 0;

	InputField* input = new InputField(
		script->getInteger(0), script->getInteger(1), script->getInteger(2),
		script->getInteger(3), script->getString(4));

	input->setCallback((CALLBACK_FUNCTION)scriptCallback);

	scriptCallbacks.push_back(script->getString(5));
	input->setUserData(scriptCallbacks.size() - 1);

	GuiSystem* gui = GuiSystem::getInstance();
	int idx = gui->addWidget(input);

	script->pushInteger(idx);
	return 1;
}

int createLabel()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("createLabel", 5))
		return 0;

	Label* label = new Label(script->getInteger(0), script->getInteger(1),
							 script->getInteger(2), script->getInteger(3),
							 script->getString(4));

	GuiSystem* gui = GuiSystem::getInstance();
	int idx = gui->addWidget(label);

	script->pushInteger(idx);
	return 1;
}

int createSprite()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("createSprite", 6))
		return 0;

	Sprite* sprite = new Sprite(script->getInteger(0), script->getInteger(1),
								script->getInteger(2), script->getInteger(3),
								script->getString(4), script->getString(5));

	GuiSystem* gui = GuiSystem::getInstance();
	int idx = gui->addWidget(sprite);

	script->pushInteger(idx);
	return 1;
}

int addWidgetToCanvas()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("addWidgetToCanvas", 2))
		return 0;

	Canvas* c = (Canvas*)script->getPointer(0);

	if (c)
		c->addWidget(script->getInteger(1));

	return 1;
}

int setLabel()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (script->getArgsNumber() != 2)
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w)
		w->setLabel(script->getString(1));

	return 1;
}

int getLabel()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("getLabel", 1))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w)
		script->pushString(w->getLabel());

	return 1;
}

int setDefaultFontSize()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setDefaultFontSize", 1))
		return 0;

	GuiSystem::getInstance()->setDefaultFontSize(script->getFloat(0));
	return 1;
}

int setWidgetPosition()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setWidgetPosition", 2))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	Vector2 vec;
	script->getFloatArray(1, vec, 2);

	if (w)
		w->setPosition(vec);

	return 1;
}

int setWidgetRotation()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setWidgetRotation", 2))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w)
		w->setRotation(script->getFloat(1));

	return 1;
}

int getWidgetRotation()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("getWidgetRotation", 1))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w)
		script->pushFloat(w->getRotation());

	return 1;
}

int getWidgetPosition()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("getWidgetPosition", 1))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w)
		script->pushFloatArray(w->getPosition(), 2);

	return 1;
}

int setWidgetVisible()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setWidgetVisible", 2))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w)
		w->setVisible(script->getBoolean(1));

	return 1;
}

int isWidgetVisible()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("isWidgetVisible", 1))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w)
		script->pushBoolean(w->isVisible());

	return 1;
}

int clearGui()
{
	GuiSystem::getInstance()->scheduleClear();
	return 1;
}

int setNormalBackground()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if (!script->isFunctionOk("setNormalBackground", 1))
		return 0;

	Vector4 vec;
	script->getFloatArray(0, vec, 4);
	GuiSystem::getInstance()->setNormalBackground(vec);
	return 1;
}

int setHoverBackground()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if (!script->isFunctionOk("setHoverBackground", 1))
		return 0;

	Vector4 vec;
	script->getFloatArray(0, vec, 4);
	GuiSystem::getInstance()->setHoverBackground(vec);
	return 1;
}

int setHighlightBackground()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if (!script->isFunctionOk("setHighlightBackground", 1))
		return 0;

	Vector4 vec;
	script->getFloatArray(0, vec, 4);
	GuiSystem::getInstance()->setHoverBackground(vec);
	return 1;
}

int enableCanvasRenderToTexture()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if (!script->isFunctionOk("enableCanvasRenderToTexture", 2))
		return 0;

	Canvas* c = (Canvas*)script->getPointer(0);

	if (c)
		c->enableRenderToTexture(script->getString(1));

	return 1;
}

int disableCanvasRenderToTexture()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	if (!script->isFunctionOk("disableCanvasRenderToTexture", 1))
		return 0;

	Canvas* c = (Canvas*)script->getPointer(0);

	if (c)
		c->disableRenderToTexture();

	return 1;
}

int createCanvas()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	Canvas* c = new Canvas;

	GuiSystem::getInstance()->addCanvas(c);
	script->pushPointer(c);
	return 1;
}

int detroyWidget()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("destroyWidget", 1))
		return 0;

	int idx = script->getInteger(0);
	GuiSystem::getInstance()->destroyWidget(idx);
	return 1;
}

int setCanvasLayer()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setCanvasLayer", 2))
		return 0;

	Canvas* c = (Canvas*)script->getPointer(0);
	int layer = script->getInteger(1);

	if (c)
		c->setLayer(layer);

	GuiSystem::getInstance()->updateLayers();

	return 1;
}

int loadSpriteSheet()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("loadSpriteSheet", 4))
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

int deleteSpriteSheet() { return 0; }

int createTile()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("createTile", 7))
		return 0;

	Tile* tile = new Tile(script->getInteger(0), script->getInteger(1),
						  script->getInteger(2), script->getInteger(3),
						  script->getString(4), script->getInteger(5),
						  script->getInteger(6));

	GuiSystem* gui = GuiSystem::getInstance();
	int idx = gui->addWidget(tile);

	script->pushInteger(idx);
	return 1;
}

int setTileSpriteSheet()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setTileSpriteSheet", 2))
		return 0;

	Tile* w = (Tile*)GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w && !strcmp(w->getStaticName(), "Tile"))
	{
		w->setTileSheet((TileSheet*)script->getPointer(1));
	}
	else
	{
		MLOG_ERROR("Given object is not of type Tile!");
	}
	return 1;
}

int setTileOffset()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setTileOffset", 2))
		return 0;

	Tile* w = (Tile*)GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (w && !strcmp(w->getStaticName(), "Tile"))
	{
		Vector2 vec;
		script->getFloatArray(1, vec, 2);
		w->setOffset(vec);
	}
	else
	{
		MLOG_ERROR("Given object is not of type Tile!");
	}
	return 1;
}

int getSpriteSize()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("getSpriteSize", 1))
		return 0;

	Sprite* w =
		(Sprite*)GuiSystem::getInstance()->getWidget(script->getInteger(0));

	if (!w)
	{
		MLOG_ERROR("Given widget is NULL!");
		return 0;
	}

	Vector2 size = w->getSize();
	script->pushFloatArray(size, 2);

	return 1;
}

int setWidgetScale()
{

	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setWidgetScale", 2))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	Vector2 vec;
	script->getFloatArray(1, vec, 2);

	if (w)
		w->setScale(vec);

	return 1;
}

int setWidgetFlip()
{

	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setWidgetFlip", 2))
		return 0;

	Widget* w = GuiSystem::getInstance()->getWidget(script->getInteger(0));

	Vector2 vec;
	script->getFloatArray(1, vec, 2);

	if (w)
		w->setFlip(vec);

	return 1;
}

int setCanvasCameraPosition()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setCanvasCameraPosition", 2))
		return 0;

	Canvas* c = (Canvas*)script->getPointer(0);

	Vector2 vec;
	script->getFloatArray(1, vec, 2);

	if (c)
		c->setCameraOffset(vec);
	else
	{
		MLOG_ERROR("Given canvas is NULL!");
		return 0;
	}

	return 1;
}

int setThemeDirectory()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();

	if (!script->isFunctionOk("setThemeDirectory", 2))
		return 0;

	GuiSystem::getInstance()->setThemeDirectory(script->getString(0));
	return 1;
}

int createSpriteBatch()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	
	SpriteBatch* s = new SpriteBatch();
	script->pushPointer(s);
	return 1;
}

int addSpriteBatchToCanvas()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	
	if (!script->isFunctionOk("addSpriteBatchToCanvas", 2))
		return 0;

	Canvas* c = (Canvas*) script->getPointer(0);
	c->addSpriteBatch((SpriteBatch*) script->getPointer(1));
	return 1;
}

int addSpriteToBatch()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	
	if (!script->isFunctionOk("addSpriteToBatch", 2))
		return 0;

	Sprite* sprite = new Sprite(0,0,0,0,script->getString(1), "");
	SpriteBatch* sb = (SpriteBatch*) script->getPointer(0);
	sb->addSprite(sprite);

	script->pushPointer(sprite);
	
	return 1;
}

// addTileToBatch(batch, width, height, path)
int addTileToBatch()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	
	if (!script->isFunctionOk("addTileToBatch", 4))
		return 0;

	Tile* sprite = new Tile(0,0, script->getFloat(1), script->getFloat(2), 
							script->getString(3), 0, 0);
	
	SpriteBatch* sb = (SpriteBatch*) script->getPointer(0);
	sb->addSprite(sprite);

	script->pushPointer(sprite);
	return 1;
}

int updateBatchSprite()
{
	ScriptContext* script = NeoEngine::getInstance()->getScriptContext();
	
	if (!script->isFunctionOk("addSpriteToBatch", 6))
		return 0;

	Widget* sprite = (Widget*) script->getPointer(0);

	sprite->setPosition(Vector2(script->getFloat(1), script->getFloat(2)));
	sprite->setScale(Vector2(script->getFloat(3), script->getFloat(4)));
	sprite->setRotation(script->getFloat(5));

	// FIXME: Faster way to determine type!
	if(strcmp(sprite->getStaticName(), "Tile") == 0)
		((Tile*) sprite)->setOffset(Vector2(script->getInteger(6), script->getInteger(7)));

	return 1;
}

void GuiSystem::setupLuaInterface(ScriptContext* script)
{
	script->addFunction("addTileToBatch", addTileToBatch);
	script->addFunction("updateBatchSprite", updateBatchSprite);
	script->addFunction("createSpriteBatch", createSpriteBatch);
	script->addFunction("addSpriteBatchToCanvas", addSpriteBatchToCanvas);
	script->addFunction("addSpriteToBatch", addSpriteToBatch);
	
	script->addFunction("setThemeDirectory", ::setThemeDirectory);
	
	script->addFunction("setCanvasCameraPosition", setCanvasCameraPosition);
	script->addFunction("setWidgetFlip", setWidgetFlip);
	script->addFunction("setWidgetScale", setWidgetScale);

	script->addFunction("enableGui", enableGui);
	script->addFunction("getMainCanvas", getMainCanvas);
	script->addFunction("createCanvas", createCanvas);
	script->addFunction("getCanvasClearColor", getCanvasClearColor);
	script->addFunction("setCanvasClearColor", setCanvasClearColor);

	script->addFunction("createButton", createButton);
	script->addFunction("createThemedButton", createThemedButton);
	
	script->addFunction("createInput", createInput);
	script->addFunction("createLabel", createLabel);
	script->addFunction("createSprite", createSprite);

	script->addFunction("getSpriteSize", getSpriteSize);

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

	script->addFunction("enableCanvasRenderToTexture",
						enableCanvasRenderToTexture);
	script->addFunction("disableCanvasRenderToTexture",
						disableCanvasRenderToTexture);

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

	map<int, WidgetId>::iterator iter = m_widgets.begin();

	for (; iter != m_widgets.end(); iter++)
	{
		id = iter->second;
		if (id.id == idx)
		{
			found = true;
			break;
		}
	}

	if (!found)
		return;

	delete id.w;
	m_widgets.erase(iter);
}

Widget* GuiSystem::getWidget(unsigned int idx) { return m_widgets[idx].w; }

void GuiSystem::draw()
{
	unsigned int width, height;

	NeoEngine::getInstance()->getSystemContext()->getScreenSize(&width,
																&height);
	Render::getInstance()->set2D(width, height);

	if (m_enabled)
	{
		for (int i = 0; i < m_canvasVector.size(); i++)
		{
			m_canvasVector[i]->draw();
		}
	}

	NeoEngine::getInstance()->getRenderingContext()->setScissor(0, 0, width,
																height);
}

void GuiSystem::update()
{
	if (m_enabled)
	{
		for (int i = 0; i < m_canvasVector.size(); i++)
		{
			m_canvasVector[i]->update();
		}
	}

	if (m_clearScheduled)
	{
		clear();
		m_clearScheduled = false;
		return;
	}
}

void GuiSystem::clear()
{
	for (int i = 1; i < m_canvasVector.size(); i++)
	{
		delete m_canvasVector[i];
	}

	m_canvasVector.clear();

	// Only leave the main canvas in there
	m_canvasVector.push_back(Canvas::getInstance());
	Canvas::getInstance()->clear();

	for (int i = 0; i < m_widgets.size(); i++)
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
	if (c)
		m_canvasVector.push_back(c);
}

void GuiSystem::updateLayers()
{
	std::sort(m_canvasVector.begin(), m_canvasVector.end(), compareCanvasLayer);
}

void GuiSystem::setThemeDirectory(const char* dir)
{
	char path[256];

	strcpy(path, dir);
	strcat(path, "/body.png");
	m_themeBody = path;

	strcpy(path, dir);
	strcat(path, "/edge.png");
	m_themeEdge = path;

	strcpy(path, dir);
	strcat(path, "/borderh.png");
	m_themeBorderH = path;
	
	strcpy(path, dir);
	strcat(path, "/borderv.png");
	m_themeBorderV = path;

	strcpy(path, dir);
	strcat(path, "/body-hover.png");
	m_themeBodyHover = path;
	
	strcpy(path, dir);
	strcat(path, "/edge-hover.png");
	m_themeEdgeHover = path;

	strcpy(path, dir);
	strcat(path, "/borderh-hover.png");
	m_themeBorderHHover = path;
	
	strcpy(path, dir);
	strcat(path, "/borderv-hover.png");
	m_themeBorderVHover = path;
	
	strcpy(path, dir);
	strcat(path, "/body-pressed.png");
	m_themeBodyPressed = path;
	
	strcpy(path, dir);
	strcat(path, "/edge-pressed.png");
	m_themeEdgePressed = path;

	strcpy(path, dir);
	strcat(path, "/borderh-pressed.png");
	m_themeBorderHPressed = path;

	strcpy(path, dir);
	strcat(path, "/borderv-pressed.png");
	m_themeBorderVPressed = path;
	
}
