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

#ifndef __THREAD_H
#define __THREAD_H

#include <functional>

namespace Neo
{
/**
 * @brief This class defines the virtual interface for a thread.
 *
 * The interface is fully virtual and needs to be implemented before it can be used.
 * @see Semaphore
 */
class NEO_CORE_EXPORT Thread
{
private:
        bool m_running; ///< Is the thread running?
public:
	Thread() {}

    /**
      * @brief The destructor stops the thread if it can and cleans it up.
      */
	~Thread() {}

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
    virtual bool Start(std::function<int (void*)> thread_func, const char* name, void* data) = 0;

    /**
     * @brief Stop Stops this thread.
     *
     * Should stop the thread immediately but does not have to since it is considered unsafe to kill a thread.
     */
    virtual void Stop() = 0;

    /**
     * @brief Waits for a return a value and blocks the running thread for that period of time.
     * @return The exit value of thread_func.
     */
    virtual int WaitForReturn() = 0;

    /**
     * @brief Returns if the thread is currently running.
     * @return Is this thread running?
     */
    bool IsRunning() { return m_running; }
    void SetRunning(bool value) { m_running = value; }

    /**
     * @brief Returns the thread ID.
     * @return The thread ID.
     */
    virtual int GetId() = 0;

	/**
	 * @brief Creates a new thread object.
	 * @return A new thread instance.
	 */
	virtual Thread* getNew() = 0;
};

/**
 * @brief This class implements the virtual interface for a semaphore mechanism.
 *
 * The interface is fully virtual and needs to be implemented before it can be used.
 */
class NEO_CORE_EXPORT Semaphore
{
public:
	Semaphore(){};
	~Semaphore(){};

    /**
	 * @brief Initializes the semaphore.
     * @param num Number of MSemaphoreWaitAndLock without unlocking will pass until the requesting thread will block. Usually set to 1.
     * @return Returns \b true on success and \b false on failure.
     */
    virtual bool Init(int num) = 0;

	/**
	 * @brief Locks the semaphore as soon as it can.
	 * Puts the running thread into sleep until it can lock the semaphore.
	 */
	virtual bool WaitAndLock() = 0;

	/**
	 * @brief Unlocks the Semaphore immediately.
	 * Causes a potential thread that called WaitAndLock to continue.
	 */
	virtual bool Unlock() = 0;

	/**
	 * @brief Creates a new semaphore object.
	 * @return The new object.
	 */
	virtual Semaphore* getNew() = 0;

};

/**
 * @brief The ThreadFactory class allows you to register any subclass of Thread and Semaphore to provide
 * the implementation of all newly created thread objects.
 *
 * Needs to be initialized at application start with proper Thread and Semaphore objects.
 *
 * \b Attention: Never use any subclass of Thread and Semaphore directly! Always use the ThreadFactory for that!
 */
class NEO_CORE_EXPORT ThreadFactory
{
	Thread* m_templateThread;
	Semaphore* m_templateSemaphore;
public:

    static ThreadFactory* getInstance();

	/**
	 * @brief Deletes all templates.
	 */
    void clear() { SAFE_DELETE(m_templateThread); SAFE_DELETE(m_templateSemaphore); }

	/**
	 * @brief Sets the thread template object with the overwritten getNew method
	 *
	 * @note The ThreadFactory takes ownership of the given pointer and might delete it later.
	 *
	 * @param thr The template.
	 */
	void setTemplateThread(Thread* thr) { m_templateThread = thr; }

	/**
	 * @brief Sets the semaphore template object with the overwritten getNew method
	 *
	 * @note The ThreadFactory takes ownership of the given pointer and might delete it later.
	 *
	 * @param sem The template.
	 */
	void setTemplateSemaphore(Semaphore* sem) { m_templateSemaphore = sem; }

	/**
	 * @brief Creates a new thread.
	 * @return The new thread or NULL if no template is registered.
	 */
	Thread* getNewThread() { if(m_templateThread) return m_templateThread->getNew(); else return NULL; }

	/**
	 * @brief Creates a new semaphore.
	 * @return The new semaphore or NULL if no template is registered.
	 */
	Semaphore* getNewSemaphore() { if(m_templateSemaphore) return m_templateSemaphore->getNew(); else return NULL; }
};
}
#endif // MTHREAD_H
