/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MSDLThread.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2014 Yannick Pflanzer <scary-squid.de>
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

#include "MSDLThread.h"
#include <cstdio>

MSDLThread::MSDLThread() :
      m_sdlThread(NULL)
    , m_running(false)
{

}

MSDLThread::~MSDLThread()
{
    Stop();
}

// TODO: Save name!
bool MSDLThread::Start(int (*thread_func)(void*), const char* name, void* data)
{
    if(m_running)
    {
        fprintf(stderr, "ERROR: Could not start thread: Thread is already running!\n");
        return false;
    }

    m_sdlThread = SDL_CreateThread(thread_func, NULL, data);
    if(!m_sdlThread)
    {
        fprintf(stderr, "ERROR: Could not start thread: %s\n", SDL_GetError());
        return false;
    }

    m_running = true;
    return true;
}

// SDL2 does not allow killing threads!
void MSDLThread::Stop()
{
    if(!m_running || m_sdlThread == NULL)
        return;

    int ret;
    SDL_WaitThread(m_sdlThread, &ret);

    m_sdlThread = NULL;
    m_running = false;
}

int MSDLThread::WaitForReturn()
{
    int ret;

    if(!m_running)
        return -1;

    SDL_WaitThread(m_sdlThread, &ret);
    m_sdlThread = NULL;

    return ret;
}

int MSDLThread::GetId()
{
    if(!m_running)
        return -1;

    return SDL_GetThreadID(m_sdlThread);
}

////////////////////////////////////////////////////////
// Implementation of MSDLSemaphore
////////////////////////////////////////////////////////

MSDLSemaphore::MSDLSemaphore() :
    m_sdlSemaphore(NULL)
{

}

MSDLSemaphore::~MSDLSemaphore()
{
    if(m_sdlSemaphore)
        SDL_DestroySemaphore(m_sdlSemaphore);
}

bool MSDLSemaphore::Init(int num)
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

bool MSDLSemaphore::WaitAndLock(MSDLSemaphore* semaphore)
{
	if (semaphore->m_sdlSemaphore)
    {
		if (SDL_SemWait(semaphore->m_sdlSemaphore) == -1)
			return false;

        return true;
    }

    return false;
}

bool MSDLSemaphore::Unlock(MSDLSemaphore* semaphore)
{
	if (semaphore->m_sdlSemaphore)
    {
		return SDL_SemPost(semaphore->m_sdlSemaphore);
    }
}
