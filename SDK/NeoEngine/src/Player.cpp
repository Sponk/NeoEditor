#include <NeoEngine.h>
#include <Player.h>

using namespace Neo;

// FIXME: Plugin::load should return a success value!
bool Player::loadPlugin(const char* name)
{
	std::shared_ptr<Plugin> p = std::make_shared<Plugin>();
	p->load(name);
	m_plugins.push_back(p);
	return true;
}

void Player::begin()
{
	NeoGame* game = m_engine->getGame();
	game->begin();	

	if(m_engine->getLevel()->getScenesNumber() == 0)
		m_engine->loadLevel("");
}

void Player::end()
{
	NeoGame* game = m_engine->getGame();
	game->end();	
}

void Player::execute(INPUT_KEYS quitKey)
{
	NeoGame* game = m_engine->getGame();
	SystemContext* system = m_engine->getSystemContext();
	InputContext* input = m_engine->getInputContext();

	while(m_engine->isActive()
		  && !input->isKeyDown(quitKey))
	{
		unsigned long int time = system->getSystemTick();
		
		game->update();
		game->draw();
		
		system->sleep(m_frameCap - (system->getSystemTick() - time));
	}
}

