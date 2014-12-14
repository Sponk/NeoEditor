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

#ifndef __GUI_SYSTEM__
#define __GUI_SYSTEM__

#include <NeoEngine.h>
#include <map>
#include <Canvas.h>
#include <string>

namespace Neo
{
namespace Gui
{
/**
 * @brief The GuiSystem class contains all functionality to set up a GUI.
 */
class GuiSystem
{
private:

	struct WidgetId
	{
		Widget* w;
		int id;
	};

    bool m_enabled;
    int m_ids;

    std::vector<Canvas*> m_canvasVector;
    std::map<int,WidgetId> m_widgets;
    std::string m_defaultFont;
    float m_defaultFontSize;

    MVector4 m_normalBackground;
    MVector4 m_hoverBackground;
    MVector4 m_highlightBackground;

	bool m_clearScheduled;

public:

    GuiSystem();
    ~GuiSystem();

    static GuiSystem* getInstance() { static GuiSystem m_instance; return &m_instance; }
    void setupLuaInterface(MScriptContext* script);
    void setEnabled(bool enabled) { m_enabled = enabled; }

    void draw();
    void update();

    const char* getDefaultFont() { return m_defaultFont.c_str(); }
    float getDefaultFontSize() { return m_defaultFontSize; }
    void setDefaultFontSize(float s) { m_defaultFontSize = s; }

    MVector4 getNormalBackground() { return m_normalBackground; }
    MVector4 getHoverBackground() { return m_hoverBackground; }
    MVector4 getHighlightBackground() { return m_highlightBackground; }

    void setNormalBackground(MVector4 color) { m_normalBackground = color; }
    void setHoverBackground(MVector4 color) { m_hoverBackground = color; }
    void setHighlightBackground(MVector4 color) { m_highlightBackground = color; }

    Widget* getWidget(unsigned int idx);
    int addWidget(Widget* w) { WidgetId id; id.w = w; id.id = ++m_ids; m_widgets[id.id] = id; return m_ids;}
    size_t getNumWidgets() { return m_widgets.size(); }

    void addCanvas(Canvas* c) { if(c) m_canvasVector.push_back(c); }
    
    void destroyWidget(int id);

    // Deletes all widgets and canvases except the main canvas.
    void clear();
	void scheduleClear() { m_clearScheduled = true; }
};
}
}

#endif
