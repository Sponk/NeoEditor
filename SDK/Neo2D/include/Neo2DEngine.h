/*
 * Copyright 2014 (C) Yannick Pflanzer <neo-engine.de>
 *
 * This file is part of Neo2D.
 *
 * Neo2D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Neo2D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Neo2D.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von Neo2D.
 *
 * Neo2D ist Freie Software: Sie können es unter den Bedingungen
 * der GNU Lesser General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * Neo2D wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU Lesser General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit
 *diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#ifndef __GUI_SYSTEM__
#define __GUI_SYSTEM__

#ifdef SWIG
#define DISOWN(a) %apply SWIGTYPE* DISOWN {a};
#else
#define DISOWN(a)
#endif

#ifdef WIN32
	#ifdef _MSC_VER
		#pragma warning(disable: 4251)
	#endif

	#if defined(NEO2D_DLL)
		#define NEO2D_EXPORT __declspec( dllexport )
	#elif defined(NEO_CORE_STATIC)
		#define NEO2D_EXPORT
	#else
		#define NEO2D_EXPORT __declspec( dllimport )
	#endif

#else
	#define NEO2D_EXPORT
#endif

#include <NeoEngine.h>
#include <map>
#include <Canvas.h>
#include <string>

namespace Neo2D
{

using namespace Neo;

/**
 * @brief The Neo2DEngine class contains all functionality to set up a GUI.
 *
 * @author Yannick Pflanzer
 */
class NEO2D_EXPORT Neo2DEngine
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
	std::map<int, WidgetId> m_widgets;
	std::string m_defaultFont;
	float m_defaultFontSize;

	Vector4 m_normalBackground;
	Vector4 m_hoverBackground;
	Vector4 m_highlightBackground;

	bool m_clearScheduled;

	String m_themeBorderH;
	String m_themeBorderV;
	String m_themeBody;
	String m_themeEdge;

	String m_themeBorderHHover;
	String m_themeBorderVHover;
	String m_themeBodyHover;
	String m_themeEdgeHover;

	String m_themeBorderHPressed;
	String m_themeBorderVPressed;
	String m_themeBodyPressed;
	String m_themeEdgePressed;

	String m_themeDirectory;

	DataManager m_fontManager;
	DataManager m_textureManager;
	
public:
	Neo2DEngine();
	~Neo2DEngine();

	FontRef* loadFont(const char * filename, unsigned int fontsize);
	TextureRef* loadTexture(const char * filename, const bool mipmap = true, const bool preload = true);

	const char* getThemeDirectory() { return m_themeDirectory.getSafeString(); }

	const char* getThemeBorderH() { return m_themeBorderH.getSafeString(); }
	const char* getThemeBorderV() { return m_themeBorderV.getSafeString(); }
	const char* getThemeBody() { return m_themeBody.getSafeString(); }
	const char* getThemeEdge() { return m_themeEdge.getSafeString(); }	

	const char* getThemeBorderHHover() { return m_themeBorderHHover.getSafeString(); }
	const char* getThemeBorderVHover() { return m_themeBorderVHover.getSafeString(); }
	const char* getThemeBodyHover() { return m_themeBodyHover.getSafeString(); }
	const char* getThemeEdgeHover() { return m_themeEdgeHover.getSafeString(); }	


	const char* getThemeBorderHPressed() { return m_themeBorderHPressed.getSafeString(); }
	const char* getThemeBorderVPressed() { return m_themeBorderVPressed.getSafeString(); }
	const char* getThemeBodyPressed() { return m_themeBodyPressed.getSafeString(); }
	const char* getThemeEdgePressed() { return m_themeEdgePressed.getSafeString(); }	

	/**
	 * @brief Changes the current GUI theme.
	 * This change only applies to newly created widgets, all existing ones
	 * will keep using the previously selected theme.
	 *
	 * Following fils are required:
	 * "/body.png", "/edge.png", "/borderh.png", "/borderv.png",
	 * "/body-hover.png", "/edge-hover.png", "/borderh-hover.png",
	 * "/borderv-hover.png", "/body-pressed.png", "/edge-pressed.png",
	 * "/borderh-pressed.png", "/borderv-pressed.png"
	 *
	 * @param dir The directory containing the theme files.
	 */
	void setThemeDirectory(const char* dir);
	
	/**
	 * @brief Returns a global instance of the GuiSystem.
	 * @return The global GuiSystem.
	 */
	static Neo2DEngine* getInstance();

	/**
	 * @brief Registers all script functions to the given context.
	 *
	 * @param script The script interface to register to.
	 */
	void setupLuaInterface(ScriptContext* script);

	/**
	 * @brief Enables or disables the GUI.
	 * @param A boolean.
	 */
	void setEnabled(bool enabled) { m_enabled = enabled; }

	/**
	 * @brief Draws all Canvases in the order the are layered in.
	 */
	void draw();

	/**
	 * @brief Updates all Canvases
	 */
	void update();

	/**
	 * @brief Returns the default font.
	 * @return The default font.
	 */
	const char* getDefaultFont() { return m_defaultFont.c_str(); }

    /**
	 * @brief Returns the default font size.
	 * @return The default font size.
	 */
	float getDefaultFontSize() { return m_defaultFontSize; }

	/**
	 * @brief Changes the default font size.
	 * @param s The default font size.
	 */
	void setDefaultFontSize(float s) { m_defaultFontSize = s; }

	/**
	 * @brief Returns the normal background color.
	 * @return The normal background.
	 */
	Vector4 getNormalBackground() { return m_normalBackground; }

	/**
	 * @brief Returns the hover background color.
	 * @return The hover background.
	 */
	Vector4 getHoverBackground() { return m_hoverBackground; }

	/**
	 * @brief Returns the highlight color.
	 * @return The highlight background.
	 */
	Vector4 getHighlightBackground() { return m_highlightBackground; }

	/**
	 * @brief Changes the normal background color.
	 * @param color The normal background.
	 */
	void setNormalBackground(Vector4 color) { m_normalBackground = color; }

	/**
	 * @brief Changes the hover background color.
	 * @param color The hover background.
	 */
	void setHoverBackground(Vector4 color) { m_hoverBackground = color; }

	/**
	 * @brief Changes the highlight background color.
	 * @param color The highlight background.
	 */
	void setHighlightBackground(Vector4 color)
	{
		m_highlightBackground = color;
	}

	/**
	 * @brief Retrieves a registered widget using its handle.
	 *
	 * @param idx The handle of the widget.
	 * @return The widget.
	 */
	Widget* getWidget(unsigned int idx);

	/**
	 * @brief Adds a widget to the internal list.
	 *
	 * @param The new widget.
	 * @return The handle of the widget.
	 */
    DISOWN(Widget* w) int addWidget(Widget* w);

	/**
	 * @brief Returns the number of registered widgets.
	 * @return The number of widgets.
	 */
	size_t getNumWidgets() { return m_widgets.size(); }

	/**
	 * @brief Adds a new Canvas to the update list.
	 * @param c The new Canvas.
	 */
    DISOWN(Canvas* c) void addCanvas(Canvas* c);

	/**
	 * @brief Returns the Canvas with the given index.
	 * @param i The index.
	 * @return The Canvas.
	 */
	Canvas* getCanvas(unsigned int i) { return m_canvasVector[i]; }

	/**
	 * @brief Sorts all Canvases to match the priority hierarchy defined
	 * by the layer mechanism.
	 */
	void updateLayers();

	/**
	 * @brief Destroys the widget with the given handle.
	 * @param id The handle.
	 */
	void destroyWidget(int id);

	/**
	 * @brief Deletes all widgets and canvases except the main canvas.
	 */
	void clear();
	void scheduleClear() { m_clearScheduled = true; }

	bool isMouseOnGui();
};
}

#endif
