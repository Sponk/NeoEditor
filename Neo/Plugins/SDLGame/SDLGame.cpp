#include "SDLGame.h"

using namespace Neo;

SDLGame::SDLGame()
{
	ConfigurationRegistry& reg = NeoEngine::getInstance()->getConfigurationRegistry();
	reg.registerVariable("g_fullscreen", [this](std::string& str) {
		int value = std::stoi(str);

		switch(value)
		{
			case 1: value = SDL_WINDOW_FULLSCREEN; break;
			case 2: value = SDL_WINDOW_FULLSCREEN_DESKTOP; break;
			default: value = 0;
		}

		SDL_SetWindowFullscreen(this->m_window, value);
	});

	m_glVersion = &reg.registerVariable("sdl_glVersion");
	m_multisample = &reg.registerVariable("sdl_multisample");

	*m_multisample = "0";
}

SDLGame::~SDLGame()
{
	ConfigurationRegistry& reg = Neo::NeoEngine::getInstance()->getConfigurationRegistry();
	reg.removeVariable("g_fullscreen");
}

void SDLGame::onBegin()
{
	SDL_version compiled;
	SDL_version linked;

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	MLOG_INFO("SDL compiled version :\t" << static_cast<int>(compiled.major) << "." << static_cast<int>(compiled.minor) << "." << static_cast<int>(compiled.patch));
	MLOG_INFO("SDL linked version :\t" << static_cast<int>(linked.major) << "." << static_cast<int>(linked.minor) << "." << static_cast<int>(linked.patch));

	// TODO: Haptic feedback
	// We don't need SDL sound. We have OpenAL for that!
	if (SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_HAPTIC & ~SDL_INIT_AUDIO) != 0)
	{
		MLOG_ERROR("SDL Error : " << SDL_GetError());
		return;
	}

	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

	unsigned int width = 1024, height = 768;
	const char* title = "SDL Game";
	m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

	if (!m_window)
	{
		MLOG_ERROR("SDL Error : " << SDL_GetError());
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, std::stoi(*m_multisample));

	ConfigurationRegistry& reg = NeoEngine::getInstance()->getConfigurationRegistry();
	if(m_glVersion->empty())
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	}
	else if(*m_glVersion == "compatibility")
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	}
	else if(*m_glVersion == "gles")
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	}

	m_context = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_context);

	// Request a higher resolution thread timer on Windows
#ifdef WIN32
	timeBeginPeriod(1);
#endif

	NeoEngine* engine = NeoEngine::getInstance();
	engine->setLevel(new Level);
	engine->getSystemContext()->setScreenSize(width, height);
	NeoGame::onBegin();
}

void SDLGame::onEnd()
{
	NeoGame::onEnd();

	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void SDLGame::update()
{
	NeoGame::update();
}

void SDLGame::draw()
{
	//SDL_GL_MakeCurrent(m_window, m_context);
	NeoEngine::getInstance()->getRenderingContext()->clear(BUFFER_COLOR);
	NeoGame::draw();
	// SDL_GL_SwapWindow(m_window);
}
