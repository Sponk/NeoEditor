#include "SDLGame.h"

using namespace Neo;

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

	if (/*m_fullscreen*/ false)
		flags = flags | SDL_WINDOW_FULLSCREEN;

	unsigned int width = 1024, height = 768;
	const char* title = "SDL Game";
	m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

	if (!m_window)
	{
		MLOG_ERROR("SDL Error : " << SDL_GetError());
		return;
	}

	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	/// FIXME: General configuration interface? Maybe in NeoEngine?
	const char* glversion = getenv("NEO_GL");
	if(glversion && !strcmp(glversion, "compat"))
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	}
	else
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
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
