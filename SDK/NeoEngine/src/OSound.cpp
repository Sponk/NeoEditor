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

// constructor
OSound::OSound(SoundRef * soundRef):
	Object3d(),
	m_sourceId(0),
	m_soundRef(soundRef)
{
	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	// create Source
	NeoEngine::getInstance()->getSoundContext()->createSource(&m_sourceId, bufferId);

	setLooping(false);
	setRelative(false);
	setPitch(1.0f);
	setGain(50.0f);
	setRadius(10.0f);
	setRolloff(1.0f);
}

// copy constructor
OSound::OSound(const OSound & sound):
	Object3d(sound),
	m_sourceId(0),
	m_soundRef(sound.m_soundRef)
{
	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	// create Source
	NeoEngine::getInstance()->getSoundContext()->createSource(&m_sourceId, bufferId);

	setLooping(sound.m_isLooping);
	setRelative(sound.m_isRelative);
	setPitch(sound.m_pitch);
	setGain(sound.m_gain);
	setRadius(sound.m_radius);
	setRolloff(sound.m_rolloff);
}

// destructor
OSound::~OSound(void)
{
	Object3d::clearObject3d();
	NeoEngine::getInstance()->getSoundContext()->deleteSource(&m_sourceId);
}

void OSound::setSoundRef(SoundRef * soundRef)
{
	m_soundRef = soundRef;

	unsigned int bufferId = 0;
	if(m_soundRef)
		bufferId = m_soundRef->getBufferId();

	NeoEngine::getInstance()->getSoundContext()->setSourceBufferId(m_sourceId, bufferId);
}

bool OSound::isPlaying(void)
{
	return NeoEngine::getInstance()->getSoundContext()->isSourcePlaying(m_sourceId);
}

float OSound::getTimePos(void)
{
	return NeoEngine::getInstance()->getSoundContext()->getSourceTimePos(m_sourceId);
}

float OSound::getSoundDuration(void)
{
	if(m_soundRef)
		return NeoEngine::getInstance()->getSoundContext()->getBufferDuration(m_soundRef->getBufferId());
	else
		return 0.0f;
}

void OSound::play(void)
{
	SoundContext * soundContext = NeoEngine::getInstance()->getSoundContext();

	if(m_soundRef)
	{
		unsigned int bufferId = m_soundRef->getBufferId();
		soundContext->setSourceBufferId(m_sourceId, bufferId);
		soundContext->playSource(m_sourceId);
	}
}

void OSound::pause(void)
{
	NeoEngine::getInstance()->getSoundContext()->pauseSource(m_sourceId);
}

void OSound::stop(void)
{
	NeoEngine::getInstance()->getSoundContext()->stopSource(m_sourceId);
}

void OSound::setLooping(bool loop)
{ 
	SoundContext * soundContext = NeoEngine::getInstance()->getSoundContext();

	if(loop != m_isLooping)
	{
		if(loop)
			soundContext->enableSourceLoop(m_sourceId);
		else
			soundContext->disableSourceLoop(m_sourceId);

		m_isLooping = loop;
	}
}

void OSound::setRelative(bool relative)
{
	m_isRelative = relative;
	NeoEngine::getInstance()->getSoundContext()->setSourceRelative(m_sourceId, m_isRelative);
}

void OSound::setPitch(float pitch)
{
	m_pitch = pitch;
	NeoEngine::getInstance()->getSoundContext()->setSourcePitch(m_sourceId, m_pitch);
}

void OSound::setGain(float gain)
{
	m_gain = gain;
	NeoEngine::getInstance()->getSoundContext()->setSourceGain(m_sourceId, m_gain);
}

void OSound::setRadius(float radius)
{
	m_radius = radius;
	NeoEngine::getInstance()->getSoundContext()->setSourceRadius(m_sourceId, m_radius);
}

void OSound::setRolloff(float rolloff)
{
	m_rolloff = rolloff;
	NeoEngine::getInstance()->getSoundContext()->setSourceRolloff(m_sourceId, m_rolloff);
}

void OSound::update(void)
{
	NeoEngine::getInstance()->getSoundContext()->setSourcePosition(m_sourceId, getTransformedPosition());
}
