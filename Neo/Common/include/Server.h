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

#ifndef __SEVER_H__
#define __SEVER_H__

#ifdef USE_NETWORKING

#include <NeoEngine.h>
#include <map>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>

namespace Neo
{

enum NetworkMessages
{
	ID_RPC_MESSAGE = ID_USER_PACKET_ENUM + 1
};

class Server
{
private:
	std::map<std::string, void (*)(RakNet::BitStream*)> m_rpcFunctions;
	MThread* m_serverThread;

	bool m_isServer;

protected:
	RakNet::RakPeerInterface* m_peer;

	unsigned int m_maxClients;
	unsigned int m_port;
	MString m_host;

	static int server_thread(void* data);

public:
	Server() : m_serverThread(NULL) {}
	void startServer(unsigned int maxClients, unsigned int port);
	void startClient(const char* host, unsigned int port);

	void addRPCFunction(const char* name, void (*func)(RakNet::BitStream*)) {m_rpcFunctions[name] = func; }
};
}

#endif

#endif
