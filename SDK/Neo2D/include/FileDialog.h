/*
 * Copyright 2015 (C) Yannick Pflanzer <neo-engine.de>
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
 * Sie sollten eine Kopie der GNU Lesser General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */

#ifndef __FILEDIALOG_H
#define __FILEDIALOG_H

#include <Window.h>
#include <ThemedButton.h>
#include <ThemedInputField.h>
#include <ScrollPane.h>
#include <functional>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Models the file dialog type.
 */
enum FILE_DIALOG_TYPE
{
	FILE_OPEN, /// File dialog that allows opening an existing file
	FILE_SAVE, /// File dialog that allows creating a new file
	DIR_OPEN /// File dialog that allows opening directories only
};

class NEO2D_EXPORT FileDialog: public Gui::Window
{
	bool m_changeDirectory;
	FILE_DIALOG_TYPE m_type;

	Button* m_okButton;
	Button* m_cancelButton;
	Button* m_directoryUpButton;
	InputField* m_addressBar;
	InputField* m_filenameInput;

	ScrollPane* m_scrollPane;

	/**
	 * @brief Clears currently active widgets and rebuilds the UI.
	 */
	void setupWidgets();

	/**
	 * @brief Callback for the "open"/"save" buttons.
	 */
	static void okCallback(Widget* button, long int data);

	/**
	 * @brief Callback for the cancel button.
	 */
	static void cancelCallback(Widget* button, long int data);

	/**
	 * @brief Callback that changes the directory relative to
	 * the current location using the label of the parent widget.
	 */
	static void changeDirectoryRelative(Widget *button, long int data);

	/**
	 * @brief Callback that changes the directory absolutely using the label of the parent widget.
	 */
	static void changeDirectoryAbsolute(Widget *button, long int data);

	/**
	 * @brief Callback that selects a file.
	 */
	static void selectFileCallback(Widget *button, long int data);

	/**
	 * @brief Callback that switches to the parent directory.
	 */
	static void directoryUpCallback(Widget* button, long int data);

	String m_curdir; /// The currently viewed directory
	String m_selectedFile; /// The currently selected file
	String m_filter; /// The currently selected file filter

public:
	FileDialog(unsigned int x, unsigned int y, const char *title, FILE_DIALOG_TYPE type, const char* startdir = "/")
		:
		m_okButton(NULL),
		m_cancelButton(NULL),
		m_scrollPane(NULL),
		m_type(type),
		m_curdir(startdir),
		m_directoryUpButton(NULL),
		m_addressBar(NULL),
		m_filenameInput(NULL),
		m_changeDirectory(true),
		Window(x, y, 600, 400, title)
	{
		setResizable(false);
		setupWidgets();
	}

	~FileDialog();

	void update();

	/**
	 * @return The currently selected file path.
	 */
	const char* getSelectedFilename();

	/**
	 * @brief Shows the given directory inside the item view.
	 * @param The directory to show.
	 */
	void readDirectory(const char* dir);

	/**
	 * @brief Positions items on the screen so everything is visible.
	 */
	void placeIcons();

	/**
	 * @return The current file type filter.
	 */
	const char* getFilter()
	{
		return m_filter.getSafeString();
	}

	/**
	 * @brief Changes the current file type filter.
	 * @param f The new filter as a regular expression.
	 */
	void setFilter(const char* f)
	{
		m_filter.set(f);
	}
};
}
}

#endif