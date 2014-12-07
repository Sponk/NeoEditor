/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMaterialsAnimRef.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#include "../Includes/NeoEngine.h"

using namespace Neo;

MaterialsAnimRef::MaterialsAnimRef(MaterialsAnim * materialsAnim, const char * filename):
	m_materialsAnim(materialsAnim){
	m_filename.set(filename);
}

MaterialsAnimRef::~MaterialsAnimRef(void){
	clear();
}

MaterialsAnimRef * MaterialsAnimRef::getNew(MaterialsAnim * materialsAnim, const char * filename)
{
	return new MaterialsAnimRef(materialsAnim, filename);
}

void MaterialsAnimRef::clear(void)
{
	if(m_materialsAnim)
	{
		m_materialsAnim->destroy();
		m_materialsAnim = NULL;
	}
}

void MaterialsAnimRef::destroy(void)
{
	delete this;
}

void MaterialsAnimRef::update(void)
{
	NeoEngine * engine = NeoEngine::getInstance();

	if(! m_materialsAnim)
		m_materialsAnim = MaterialsAnim::getNew();

	engine->getMaterialsAnimLoader()->loadData(getFilename(), m_materialsAnim);	
}
