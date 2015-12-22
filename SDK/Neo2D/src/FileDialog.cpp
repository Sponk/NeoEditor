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

#include <FileDialog.h>
#include <DraggableIcon.h>
#include <NeoCore.h>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>

#include <sys/stat.h>
#include <sys/types.h>

#define DEFAULT_SIZE 64

using namespace Neo2D;
using namespace Gui;

FileDialog::~FileDialog()
{
	// FIXME: Put into "clear" method!
	// Clear widgets
	SAFE_DELETE(m_okButton);
	SAFE_DELETE(m_cancelButton);
	SAFE_DELETE(m_scrollPane);

	m_content.clear();
}

void FileDialog::setupWidgets()
{
	// Clear widgets
	SAFE_DELETE(m_okButton);
	SAFE_DELETE(m_cancelButton);
	SAFE_DELETE(m_scrollPane);
	SAFE_DELETE(m_directoryUpButton);
	SAFE_DELETE(m_addressBar);
	SAFE_DELETE(m_filenameInput);

	m_content.clear();

	// TODO: Icon button!
	m_directoryUpButton = new ThemedButton(10, 10, 20, 20, "..");
	m_okButton = new ThemedButton(m_width - 60, m_height - 30, 50, 20, (m_type == FILE_SAVE) ? "Save" : "Open");
	m_cancelButton = new ThemedButton(m_width - 120, m_height - 30, 50, 20, "Cancel");

	m_directoryUpButton->setCallback(FileDialog::directoryUpCallback, (long int) this);
	m_okButton->setCallback(FileDialog::okCallback, (long int) this);
	m_cancelButton->setCallback(FileDialog::cancelCallback, (long int) this);

	m_addressBar = new ThemedInputField(50, 10, m_width - 70, 20, m_curdir.getSafeString());
	m_addressBar->setCallback(FileDialog::changeDirectoryAbsolute, (long int) this);

	m_filenameInput = new ThemedInputField(10, m_height - 30, m_width - 190, 20, "");
	m_filenameInput->setCallback(FileDialog::selectFileCallback, (long int) this);

	m_scrollPane = new ScrollPane(0, 40, m_width, m_height - 80);

	addWidget(m_okButton);
	addWidget(m_cancelButton);
	addWidget(m_scrollPane);
	addWidget(m_directoryUpButton);
	addWidget(m_addressBar);
	addWidget(m_filenameInput);

	// Read start directory
	readDirectory(m_curdir.getSafeString());

	// Update widgets that need it for init
	m_scrollPane->update();
	m_addressBar->update();
}

void FileDialog::okCallback(Widget* button, long int data)
{
	FileDialog* dlg = (FileDialog*) data;

	if(dlg->m_type == DIR_OPEN)
		dlg->m_selectedFile.set(dlg->m_curdir.getSafeString());

	dlg->doCallback();
	dlg->setVisible(false);
}

void FileDialog::cancelCallback(Widget* button, long int data)
{
	((FileDialog*) data)->doCallback();
	((FileDialog*) data)->setVisible(false);
}

const char* FileDialog::getSelectedFilename()
{
	return m_selectedFile.getSafeString();
}

bool lexCompare(std::string a, std::string b)
{
	return a < b;
}

void FileDialog::readDirectory(const char *dir)
{
	for(Widget* w : m_scrollPane->getContent())
		delete w;

	m_scrollPane->getContent().clear();

	std::vector<std::string> directory;
	::readDirectory(dir, directory, false);

	// Sort entries lexicographically
	std::sort(directory.begin(), directory.end(), lexCompare);

	// Create filter regex
	std::regex reg(m_filter.getSafeString());

	m_curdir.set(dir);
	m_addressBar->setLabel(dir);

	std::string themedir = Neo2DEngine::getInstance()->getThemeDirectory();

	struct stat st;
	std::string start = (!strcmp(dir, "/")) ? "" : dir;

	std::string fullpath;
	for(std::string& s : directory)
	{
		fullpath = start + "/" + s;

		if(!stat(fullpath.c_str(), &st))
		{
			DraggableIcon* icon;
			if(S_ISDIR(st.st_mode))
			{

				icon = new DraggableIcon(100, 100, DEFAULT_SIZE, DEFAULT_SIZE, (themedir + "/icons/folder.png").c_str(), s.c_str());
				icon->setCallback(FileDialog::changeDirectoryRelative, (long int) this);
				// MLOG_INFO("<DIR>\t" << fullpath);
			}
			else if(std::regex_match(s, reg) && (m_type == FILE_OPEN || m_type == FILE_SAVE))
			{
				icon = new DraggableIcon(100, 100, DEFAULT_SIZE, DEFAULT_SIZE, (themedir + "/icons/text-x-generic.png").c_str(), s.c_str());
				icon->setCallback(FileDialog::selectFileCallback, (long int) this);
				icon->setClickMode(SINGLE_CLICK);
				// MLOG_INFO("<FILE>\t" << fullpath);
			}
			else
				continue;

			//icon->set
			icon->rotate(180);
			m_scrollPane->addWidget(icon);
		}
	}

	placeIcons();
}

void FileDialog::placeIcons()
{
	unsigned int x = 10, y = 10;
	Vector2 size;
	for(Widget* w : m_scrollPane->getContent())
	{
		w->setPosition(Neo::Vector2(x, y));

		x += DEFAULT_SIZE + 10;
		if(x >= m_scrollPane->getSize().x - DEFAULT_SIZE)
		{
			x = 10;
			y += DEFAULT_SIZE + 15;
		}
	}
}

void FileDialog::changeDirectoryRelative(Widget *button, long int data)
{
	FileDialog* dlg = (FileDialog*) data;
	std::string nextdir = dlg->m_curdir.getSafeString();

	if(nextdir != "/")
		nextdir += "/";

	nextdir += button->getLabel();

	// MLOG_INFO("Changing to: " << nextdir);

	// Request changing directories
	dlg->m_curdir.set(nextdir.c_str());
	dlg->m_changeDirectory = true;
	//dlg->readDirectory(nextdir.c_str());
}

void FileDialog::changeDirectoryAbsolute(Widget *button, long int data)
{
	FileDialog* dlg = (FileDialog*) data;
	dlg->m_curdir.set(button->getLabel());
	dlg->m_changeDirectory = true;

	if(dlg->m_type == DIR_OPEN)
		dlg->m_selectedFile.set(dlg->m_curdir.getSafeString());
}

void FileDialog::update()
{
	if(m_changeDirectory)
		readDirectory(m_curdir.getSafeString());

	m_changeDirectory = false;

	Window::update();
}

void FileDialog::directoryUpCallback(Widget *button, long int data)
{
	FileDialog* dlg = (FileDialog*) data;
	std::string nextdir = dlg->m_curdir.getSafeString();

	// MLOG_INFO("Changing to: " << nextdir);

	int idx = nextdir.find_last_of("/");

	if(idx != -1 && nextdir != "/")
		nextdir.erase((idx == 0) ? idx+1 : idx);
	else
		return;

	dlg->readDirectory(nextdir.c_str());
}

void FileDialog::selectFileCallback(Widget *button, long int data)
{
	FileDialog* dlg = (FileDialog*) data;

	std::string base = dlg->m_curdir.getSafeString();
	base += (!strcmp(dlg->m_curdir.getSafeString(), "/")) ? "" : "/";

	dlg->m_selectedFile.set((base + button->getLabel()).c_str());

	if(button != dlg->m_filenameInput)
		dlg->m_filenameInput->setLabel(button->getLabel());
}
