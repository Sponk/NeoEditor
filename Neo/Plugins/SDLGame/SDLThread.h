//========================================================================
// Copyright (c) 2014-2016 Yannick Pflanzer <neo-engine.de>
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

#ifndef SDLTHREAD_H
#define SDLTHREAD_H

#include <NeoCore.h>
#include <SDL.h>

class SDLThread: public Neo::Thread
{
private:
	SDL_Thread* m_sdlThread; ///< The SDL thread
	bool m_running;
public:
	SDLThread();
	~SDLThread();

	bool Start(std::function<int(void*)> thread_func, const char* name, void* data);
	void Stop();
	int WaitForReturn();
	bool IsRunning() { return m_running; }
	void SetRunning(bool value) { m_running = value; }
	int GetId();

	Thread* getNew() { return new SDLThread(); }
};

class SDLSemaphore: public Neo::Semaphore
{
	SDL_semaphore* m_sdlSemaphore; ///< The SDL semaphore
public:
	SDLSemaphore();
	~SDLSemaphore();

	bool Init(int num);
	bool WaitAndLock();
	bool Unlock();

	Neo::Semaphore* getNew() { return new SDLSemaphore(); }
};

#endif