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
class Server
{
private:
	std::vector<Client> m_clients;
	std::map<std::string, void (*)()> m_rpcFunctions;
	MThread* m_serverThread;

protected:
	RakNet::RakPeerInterface* m_peer;

	unsigned int m_maxClients;
	unsigned int m_port;

	static int server_thread(void* data);

public:
	Server() : m_serverThread(NULL) {}
	void start(unsigned int maxClients, unsigned int port);

	void addRPCFunction(const char* name, void (*func)()) {m_rpcFunctions[name] = func; }
};
}

#endif

#endif
