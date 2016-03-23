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


#include <NeoEngine.h>

using namespace Neo;

ArmatureAnimRef::ArmatureAnimRef(ArmatureAnim * armatureAnim, const char * filename):
	m_armatureAnim(armatureAnim){
	m_filename = filename;
}

ArmatureAnimRef::~ArmatureAnimRef(void){
	clear();
}

ArmatureAnimRef * ArmatureAnimRef::getNew(ArmatureAnim * armatureAnim, const char * filename){
	return new ArmatureAnimRef(armatureAnim, filename);
}

void ArmatureAnimRef::clear(void)
{
	if(m_armatureAnim)
	{
		m_armatureAnim->destroy();
		m_armatureAnim = NULL;
	}
}

void ArmatureAnimRef::destroy(void){
	delete this;
}

void ArmatureAnimRef::update(void)
{
	NeoEngine * engine = NeoEngine::getInstance();

	if(! m_armatureAnim)
		m_armatureAnim = ArmatureAnim::getNew();

	engine->getArmatureAnimLoader()->loadData(getFilename(), m_armatureAnim);	
}
