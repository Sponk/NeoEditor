#ifndef __PLAYER_H
#define __PLAYER_H

#include <memory>

namespace Neo
{
/**
 * @brief Implements a basic player.
 *
 * Each player contains its own NeoEngine instance and runs at a configurable
 * speed. Additionally each instance handles a seperate set of loaded plugins.
 */
class NEO_ENGINE_EXPORT Player
{
	unsigned int m_frameCap;
	std::vector<std::shared_ptr<Plugin>> m_plugins;
	NeoEngine* m_engine;

 public:
	Player(unsigned int framecap) : m_frameCap(framecap)
	{
		// Cache current instance
		m_engine = NeoEngine::getInstance();
		//NeoEngine::setInstance(&m_engine);
	}
	
	bool loadPlugin(const char* name);

	void begin();
	void execute(INPUT_KEYS quitKey = KEY_DUMMY);
	void end();
};
}

#endif
