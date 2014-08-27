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

/**
 * @brief The MThread class implements a multithreading mechanism
 * @bug Should this class be here, in MEngine or somewhere else?
 *
 * @see MSemaphore
 * @see MSleep
 */
class M_CORE_EXPORT MThread
{
private:
        bool m_running; ///< Is the thread running?
public:
	MThread() {}

    /**
      * @brief The destructor calls MThread::Stop to clean up.
      */
	~MThread() {}

    /**
     * @brief Start Starts the function given as argument in this thread.
     *
     * The data argument will be given to the thread function when the thread starts.
     *
     * The function has to look like:
     * \code
     * int thread_func(void*)
     * \endcode
     *
     * @param thread_func Function that will be executed when thread starts.
     * @param name The name of the thread. For example 'input' or 'update'
     * @param data Data that needs to be pushed to the thread function.
     * @return Returns \b true on success, \b false on failure.
     */
    virtual bool Start(int (*thread_func)(void*), const char* name, void* data) = 0;

    /**
     * @brief Stop Stops this thread.
     *
     * This would force quit the thread on older versions of SDL. In this version (2.x) it
     * only calls MThread::WaitForReturn and cleans up all private variables.
     */
    virtual void Stop() = 0;

    /**
     * @brief WaitForReturn Waits for thread_func from MThread::Start to return a value.
     * @return The exit value of thread_func.
     */
    virtual int WaitForReturn() = 0;

    /**
     * @brief IsRunning Returns if the thread is currently running.
     * @return Is this thread running?
     */
    bool IsRunning() { return m_running; }
    void SetRunning(bool value) { m_running = value; }

    /**
     * @brief GetId Gets the thread ID from SDL and returns it.
     * @return The thread ID.
     */
    virtual int GetId() = 0;
};

/**
 * @brief The MSemaphore class implements a semaphore mechanism based on SDL for use with MThread.
 * @bug Should be in its own file!
 *
 * @see MSemaphoreWaitAndLock
 * @see MSemaphoreUnlock
 */
class M_CORE_EXPORT MSemaphore
{
public:
	MSemaphore(){};
	~MSemaphore(){};

    /**
     * @brief Initializes the SDL semaphore.
     * @param num Number of MSemaphoreWaitAndLock without unlocking will pass until the requesting thread will block. Usually set to 1.
     * @return Returns \b true on success and \b false on failure.
     */
    virtual bool Init(int num) = 0;

	/**
	* @brief Locks the semaphore given as parameter and waits if it's already locked.
	*
	* @param sem The pointer to a MSemaphore which shall be locked/waited for.
	* @return Returns \b true on success and \b false on failure.
	*/
	static bool WaitAndLock(MSemaphore*);
	static bool Unlock(MSemaphore*);
};

#endif // MTHREAD_H
