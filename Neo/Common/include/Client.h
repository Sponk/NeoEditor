#ifndef __CLIENT_H__
#define __CLIENT_H__

#ifdef USE_NETWORKING

#include <MCore.h>
#include <RakPeerInterface.h>
#include <NeoEngine.h>
#include <MessageIdentifiers.h>

namespace Neo
{

enum NetworkMessages
{
	ID_RPC_MESSAGE = ID_USER_PACKET_ENUM+1
};

class Client
{
private:
	MThread* m_clientThread;

protected:
	RakNet::RakPeerInterface* m_peer;
	unsigned int m_port;
	MString m_host;

	static int client_thread(void* data);

public:
	Client() : m_clientThread(NULL) {}
	void start(const char* host, unsigned int port);
};
}
#endif
#endif
