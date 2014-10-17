/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MFilesUpdate.h
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


#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "MFilesUpdate.h"


void updateRefs(MDataManager * dataManager)
{
	unsigned int i;
	unsigned int size = dataManager->getRefsNumber();
	for(i=0; i<size; i++)
	{
		MDataRef * ref = dataManager->getRef(i);
		const char * filename = ref->getFilename();
		if(filename)
		{
			struct stat timeBuf;
			if(stat(filename, &timeBuf) == 0)
			{
				long time = (long)timeBuf.st_mtime;
				if(ref->getTime() != time)
				{
					ref->update();
					ref->setTime(time);
				}
			}
		}
	}
}

void MFilesUpdate::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();

	MDataManager * textureManager = level->getTextureManager();
	MDataManager * shaderManager = level->getShaderManager();
	MDataManager * soundManager = level->getSoundManager();
	MDataManager * meshManager = level->getMeshManager();
	MDataManager * armatureAnimManager = level->getArmatureAnimManager();
	MDataManager * texturesAnimManager = level->getTexturesAnimManager();
	MDataManager * materialsAnimManager = level->getMaterialsAnimManager();

	updateRefs(textureManager);
	updateRefs(shaderManager);
	updateRefs(meshManager);
	updateRefs(soundManager);
	updateRefs(armatureAnimManager);
	updateRefs(texturesAnimManager);
	updateRefs(materialsAnimManager);
}