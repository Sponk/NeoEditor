/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MSoundRef.cpp
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

SoundRef::SoundRef(unsigned int bufferId, const char * filename):
	m_bufferId(bufferId){
	m_filename.set(filename);
}

SoundRef::~SoundRef(void){
	clear();
}

SoundRef * SoundRef::getNew(unsigned int bufferId, const char * filename){
	return new SoundRef(bufferId, filename);
}

void SoundRef::clear(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();

	soundContext->deleteBuffer(&m_bufferId);
}

void SoundRef::destroy(void){
	delete this;
}

void SoundRef::update(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	MSoundContext * soundContext = engine->getSoundContext();

	// force clear
	clear();

	MSound sound;
	if(engine->getSoundLoader()->loadData(getFilename(), &sound))
	{
		// create new buffer
		soundContext->createBuffer(&m_bufferId);
		soundContext->sendBufferSound(m_bufferId, &sound);
	}	
}
