#ifndef __SEVER_H__
#define __SEVER_H__

#ifdef USE_NETWORKING

#include <NeoEngine.h>
#include <Client.h>
#include <map>
#include <string>

#include <RakPeerInterface.h>

namespace Neo
{

enum NetworkMessages
{
	ID_RPC_MESSAGE = ID_USER_PACKET_ENUM + 1
};

class Server
{
private:
	std::vector<Client> m_clients;
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
