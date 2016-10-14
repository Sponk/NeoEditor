//========================================================================
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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
#include <Player.h>
#include <algorithm>

using namespace Neo;

// FIXME: Plugin::load should return a success value!
bool Player::loadPlugin(const char* name)
{
	std::shared_ptr<Plugin> p = std::make_shared<Plugin>();
	p->load(name);
	m_plugins.push_back(p);
	return true;
}

void Player::begin(const char* level)
{
	NeoGame* game = m_engine->getGame();
	game->begin();

	if (m_engine->getLevel() == NULL)
	{
		m_engine->setLevel(new Level);
		m_engine->loadLevel((level == NULL) ? "" : level);
		m_engine->getScriptContext()->runScript(m_engine->getLevel()->getCurrentScene()->getScriptFilename());
	}
}

void Player::end()
{
	NeoGame* game = m_engine->getGame();
	game->end();
	
	m_engine->getLevel()->clear();
}

void Player::execute(INPUT_KEYS quitKey)
{
	SystemContext* system = m_engine->getSystemContext();
	InputContext* input = m_engine->getInputContext();

	while(m_engine->isActive())
	{
		unsigned long int time = system->getSystemTick();
		
		executeFrame();
		
		system->sleep(m_frameCap - (system->getSystemTick() - time));
		m_engine->setActive(m_engine->isActive() && (quitKey == KEY_DUMMY || !input->isKeyDown(quitKey)));
	}
}

void Player::executeFrame()
{
	NeoGame* game = m_engine->getGame();
	game->update();
	game->startFrame();
	game->draw();
	game->endFrame();
}
