/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MaratisPlayer
// MaratisPlayer.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================


#ifndef _MARATIS_PLAYER_H
#define _MARATIS_PLAYER_H

#include <MEngine.h>
#include <MPlugin/MPlugin.h>
#include <MProject/MProject.h>


class MaratisPlayer
{
public:

	// constructor / destructor
	MaratisPlayer(void);
	~MaratisPlayer(void);

	// instance
	static MaratisPlayer * getInstance(void)
	{
		static MaratisPlayer m_instance;
		return &m_instance;
	}

private:

	// MEngine
	SoundContext * m_soundContext;
	RenderingContext * m_render;
	PhysicsContext * m_physics;
	ScriptContext * m_script;
	InputContext * m_input;
	SystemContext * m_system;
	NeoGame * m_game;
	Level * m_level;
	Renderer * m_renderer;
	MPackageManager * m_packageManager;

	// plugins
	std::vector <MPlugin *> m_plugins;

	// game plugin
	MPlugin * m_gamePlugin;

private:

	// start
	void start(void);

public:

	// renderer
	void changeRenderer(const char * name);

	// game plugin
	void loadGamePlugin(void);

	// restart
	void restart(void);

	// clear
	void clear(void);

	// project load
	bool loadProject(const char * filename);
	void loadProject(MProject * proj, const char * filename);
	
	// main loops
	static void logicLoop(void);
	static void graphicLoop(void);
};

#endif