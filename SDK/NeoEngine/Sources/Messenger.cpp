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

#include <Messenger.h>

using namespace Neo;

void Messenger::addInbox(const char* name, unsigned int id)
{
	Inbox box;
	box.name = name;
	box.id = id;

	m_boxes[std::string(name)] = box;
}

unsigned int Messenger::getMessagesCount(const char* name)
{
	return m_boxes[std::string(name)].messages.size();
}

void Messenger::sendMessage(const char* message, unsigned int messageId, const char* dest, unsigned int from)
{
	Message msg;
	msg.message = message;
	msg.messageId = messageId;
	msg.sender = from;

	m_boxes[dest].messages.push_back(msg);
}

Message Messenger::getNextMessage(const char* threadName)
{
	Message msg = m_boxes[threadName].messages.front();
	m_boxes[threadName].messages.pop_front();

	return msg;
}
