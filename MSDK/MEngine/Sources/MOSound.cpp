/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MOSound.cpp
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


#include "../Includes/MEngine.h"


// constructor
MOSound::MOSound(MSoundRef * soundRef):
	MObject3d(),
	m_sourceId(0),
	m_soundRef(soundRef)
{
	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	// create Source
	MEngine::getInstance()->getSoundContext()->createSource(&m_sourceId, bufferId);

	setLooping(false);
	setRelative(false);
	setPitch(1.0f);
	setGain(50.0f);
	setRadius(10.0f);
	setRolloff(1.0f);
}

// copy constructor
MOSound::MOSound(const MOSound & sound):
	MObject3d(sound),
	m_sourceId(0),
	m_soundRef(sound.m_soundRef)
{
	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	// create Source
	MEngine::getInstance()->getSoundContext()->createSource(&m_sourceId, bufferId);

	setLooping(sound.m_isLooping);
	setRelative(sound.m_isRelative);
	setPitch(sound.m_pitch);
	setGain(sound.m_gain);
	setRadius(sound.m_radius);
	setRolloff(sound.m_rolloff);
}

// destructor
MOSound::~MOSound(void)
{
	MObject3d::clearObject3d();
	MEngine::getInstance()->getSoundContext()->deleteSource(&m_sourceId);
}

void MOSound::setSoundRef(MSoundRef * soundRef)
{
	m_soundRef = soundRef;

	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	MEngine::getInstance()->getSoundContext()->setSourceBufferId(m_sourceId, bufferId);
}

bool MOSound::isPlaying(void)
{
	return MEngine::getInstance()->getSoundContext()->isSourcePlaying(m_sourceId);
}

float MOSound::getTimePos(void)
{
	return MEngine::getInstance()->getSoundContext()->getSourceTimePos(m_sourceId);
}

float MOSound::getSoundDuration(void)
{
	if(m_soundRef)
		return MEngine::getInstance()->getSoundContext()->getBufferDuration(m_soundRef->getBufferId());
	else
		return 0.0f;
}

void MOSound::play(void)
{
	MSoundContext * soundContext = MEngine::getInstance()->getSoundContext();

	if(m_soundRef)
	{
		unsigned int bufferId = m_soundRef->getBufferId();
		soundContext->setSourceBufferId(m_sourceId, bufferId);
		soundContext->playSource(m_sourceId);
	}
}

void MOSound::pause(void)
{
	MEngine::getInstance()->getSoundContext()->pauseSource(m_sourceId);
}

void MOSound::stop(void)
{
	MEngine::getInstance()->getSoundContext()->stopSource(m_sourceId);
}

void MOSound::setLooping(bool loop)
{ 
	MSoundContext * soundContext = MEngine::getInstance()->getSoundContext();

	if(loop != m_isLooping)
	{
		if(loop)
			soundContext->enableSourceLoop(m_sourceId);
		else
			soundContext->disableSourceLoop(m_sourceId);

		m_isLooping = loop;
	}
}

void MOSound::setRelative(bool relative)
{
	m_isRelative = relative;
	MEngine::getInstance()->getSoundContext()->setSourceRelative(m_sourceId, m_isRelative);
}

void MOSound::setPitch(float pitch)
{
	m_pitch = pitch;
	MEngine::getInstance()->getSoundContext()->setSourcePitch(m_sourceId, m_pitch);
}

void MOSound::setGain(float gain)
{
	m_gain = gain;
	MEngine::getInstance()->getSoundContext()->setSourceGain(m_sourceId, m_gain);
}

void MOSound::setRadius(float radius)
{
	m_radius = radius;
	MEngine::getInstance()->getSoundContext()->setSourceRadius(m_sourceId, m_radius);
}

void MOSound::setRolloff(float rolloff)
{
	m_rolloff = rolloff;
	MEngine::getInstance()->getSoundContext()->setSourceRolloff(m_sourceId, m_rolloff);
}

void MOSound::update(void)
{
	MEngine::getInstance()->getSoundContext()->setSourcePosition(m_sourceId, getTransformedPosition());
}