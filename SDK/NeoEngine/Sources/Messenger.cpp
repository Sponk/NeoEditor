//========================================================================
// Copyright (c) 2014 Yannick Pflanzer <neo-engine.de>
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
#include <MCore.h>

using namespace Neo;

void Messenger::addInbox(const char* name, unsigned int id)
{
    waitForAccess();

	Inbox box;
	box.name = name;
	box.id = id;

	m_boxes[name] = box;
    finishAccess();
}

unsigned int Messenger::getMessagesCount(const char* name)
{
    waitForAccess();
	unsigned int sz = m_boxes[name].messages.size();
    finishAccess();

	return sz;
}

void Messenger::sendMessage(const char* message, unsigned int messageId, void* data, const char* dest, const char* from)
{
    waitForAccess();

	Message msg;
	msg.message = message;
	msg.messageId = messageId;
	msg.sender = from;
	msg.data = data;

	m_boxes[dest].messages.push_back(msg);
	finishAccess();
}

Message Messenger::getNextMessage(const char* threadName)
{
	waitForAccess();
	Message msg = m_boxes[threadName].messages.front();
	m_boxes[threadName].messages.pop_front();
	finishAccess();

	return msg;
}

Message Messenger::getNextMessage(const char* threadName, const char* sender)
{
	waitForAccess();
	Inbox* box = &m_boxes[threadName];
	Message msg;
	msg.messageId = -1;

	for(int i = 0; i < box->messages.size(); i++)
	{
		if((msg = box->messages[i]).sender == sender)
		{
			finishAccess();
			box->messages.erase(box->messages.begin()+i);
			return msg;
		}
	}

	finishAccess();
	return msg;
}

void Messenger::waitForAccess()
{
    if(m_semaphore == NULL)
	{
		m_semaphore = MThreadManager::getInstance()->getNewSemaphore();
        m_semaphore->Init(1);
    }

    m_semaphore->WaitAndLock();
}

void Messenger::finishAccess()
{
    if(m_semaphore == NULL)
	{
		m_semaphore = MThreadManager::getInstance()->getNewSemaphore();
        m_semaphore->Init(1);
	}

    m_semaphore->Unlock();
}
