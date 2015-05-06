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

#include <NeoEngine.h>
#include <Window/Window.h>
#include <SDLThread.h>

#include <gtest/gtest.h>
#include <NeoCore.h>

using namespace Neo;

class TestThreading: public testing::Test
{
public:

	void SetUp()
	{
		NeoEngine* engine = NeoEngine::getInstance();

		// Init default thread
		ThreadFactory* mgr = ThreadFactory::getInstance();
		mgr->setTemplateSemaphore(new SDLSemaphore());
		mgr->setTemplateThread(new SDLThread());
	}

	void TearDown()
	{
		ThreadFactory::getInstance()->clear();
	}
};

int test_thread(void* pointer)
{
	if(pointer != (void*) 0xDEADBEEF)
		return 1;

	return 0;
}

TEST_F(TestThreading, CreateThread_test)
{
	ThreadFactory* mgr = ThreadFactory::getInstance();
	Thread* thrd = mgr->getNewThread();

	ASSERT_NE((void*)NULL, thrd);

	void* p = (void*) 0xDEADBEEF;
	thrd->Start(test_thread, "TestThread", p);

	int result = thrd->WaitForReturn();
	EXPECT_EQ(0, result);
}

int test_semaphore_thread(void* sem)
{
	Semaphore* semaphore = (Semaphore*) sem;
	semaphore->WaitAndLock();

	return 1;
}

TEST_F(TestThreading, Semaphore_test)
{
	ThreadFactory* mgr = ThreadFactory::getInstance();
	Thread* thrd = mgr->getNewThread();
	Semaphore* sem = mgr->getNewSemaphore();

	ASSERT_NE((void*)NULL, thrd);
	ASSERT_NE((void*)NULL, sem);

	// Init and lock the semaphore
	sem->Init(1);
	sem->WaitAndLock();

	// Start the test thread
	thrd->Start(test_semaphore_thread, "SemaphoreThread", sem);

	// Wait for some time.
	// The thread should not be able to continue while the semaphore is locked.
	NeoWindow::getInstance()->sleep(10);

	// Unlock the semaphore
	sem->Unlock();

	// Wait for the thread to finish
	int result = thrd->WaitForReturn();

	EXPECT_EQ(1, result);
}
