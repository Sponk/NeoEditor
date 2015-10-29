//========================================================================
// Copyright (c) 2014-2015 Yannick Pflanzer <neo-engine.de>
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

#include "SDLThread.h"
#include <cstdio>

using namespace Neo;

SDLThread::SDLThread() :
      m_sdlThread(NULL)
    , m_running(false)
{

}

SDLThread::~SDLThread()
{
    Stop();
}

// TODO: Save name!
bool SDLThread::Start(std::function<int (void*)> thread_func, const char* name, void* data)
{
    if(m_running)
    {
        fprintf(stderr, "ERROR: Could not start thread: Thread is already running!\n");
        return false;
    }

    m_sdlThread = SDL_CreateThread(*thread_func.target<SDL_ThreadFunction>(), NULL, data);
    if(!m_sdlThread)
    {
        fprintf(stderr, "ERROR: Could not start thread: %s\n", SDL_GetError());
        return false;
    }

    m_running = true;
    return true;
}

// SDL2 does not allow killing threads!
void SDLThread::Stop()
{
    if(!m_running || m_sdlThread == NULL)
        return;

    int ret;
    SDL_WaitThread(m_sdlThread, &ret);

    m_sdlThread = NULL;
    m_running = false;
}

int SDLThread::WaitForReturn()
{
    int ret;

    if(!m_running)
        return -1;

    SDL_WaitThread(m_sdlThread, &ret);
    m_sdlThread = NULL;

    return ret;
}

int SDLThread::GetId()
{
    if(!m_running)
        return -1;

    return SDL_GetThreadID(m_sdlThread);
}

////////////////////////////////////////////////////////
// Implementation of SDLSemaphore
////////////////////////////////////////////////////////

SDLSemaphore::SDLSemaphore() :
    m_sdlSemaphore(NULL)
{

}

SDLSemaphore::~SDLSemaphore()
{
    if(m_sdlSemaphore)
        SDL_DestroySemaphore(m_sdlSemaphore);
}

bool SDLSemaphore::Init(int num)
{
    if(m_sdlSemaphore)
    {
        fprintf(stderr, "ERROR: Could not initialize semaphore!\n");
        return false;
    }

    m_sdlSemaphore = SDL_CreateSemaphore(num);

    if(!m_sdlSemaphore)
    {
        fprintf(stderr, "ERROR: Could not initialize semaphore: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool SDLSemaphore::WaitAndLock()
{
	if (m_sdlSemaphore)
	{
		if (SDL_SemWait(m_sdlSemaphore) == -1)
			return false;

		return true;
	}

	return false;
}

bool SDLSemaphore::Unlock()
{
	if (m_sdlSemaphore)
	{
		return SDL_SemPost(m_sdlSemaphore);
	}
}
