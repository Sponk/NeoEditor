/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MThread.h
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

#ifndef MTHREAD_H
#define MTHREAD_H

#include <SDL2/SDL.h>

class MThread
{
private:
        SDL_Thread* m_sdlThread;
        bool m_running;
public:
    MThread();
    ~MThread();

    bool Start(int (*thread_func)(void*), const char* name, void* data);
    void Stop();
    int WaitForReturn();

    bool IsRunning() { return m_running; }
    int GetId();
};

class MSemaphore
{
    SDL_semaphore* m_sdlSemaphore;
public:
    MSemaphore();
    ~MSemaphore();
    bool Init(int num);
    SDL_semaphore* GetHandle() { return m_sdlSemaphore; }
};

bool MSemaphoreWaitAndLock(MSemaphore* sem);
void MSemaphoreUnlock(MSemaphore* sem);

void MSleep(int t);

#endif // MTHREAD_H
