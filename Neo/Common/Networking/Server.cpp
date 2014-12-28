#include <Server.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

using namespace Neo;
using namespace RakNet;

int Server::server_thread(void *data)
{
	Server* server = (Server*) data;
	NeoEngine* engine = NeoEngine::getInstance();
	NeoGame* game = engine->getGame();

	if(!server)
		return -1;

	MLOG_INFO("Starting network server thread");

	RakPeerInterface* peer = server->m_peer = RakPeerInterface::GetInstance();

	SocketDescriptor sd(server->m_port, 0);
	peer->Startup(server->m_maxClients, &sd, 1);
	peer->SetMaximumIncomingConnections(server->m_maxClients);

	Packet* packet;

	while(game->isRunning())
	{
		for(packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
		{
			switch(packet->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
						MLOG_INFO("New client connected: " << packet->systemAddress.ToString(true, ':'));
					break;

				case ID_RPC_MESSAGE:
					{
						RakString messageStr;

						BitStream in(packet->data, packet->length, false);
						in.IgnoreBytes(sizeof(MessageID));
						in.Read(messageStr);

						MLOG_INFO("RPC-Request from " << packet->systemAddress.ToString(true, ':'));

						void (*func)() = server->m_rpcFunctions[messageStr.C_String()];

						if(func)
							((void (*)()) func)();
						else
							MLOG_ERROR("Requested call does not exitst!");

					}
					break;

				default:
						MLOG_INFO("Message with identifier " << (int) packet->data[0] << " has arrived.");
			}
		}
	}

	RakPeerInterface::DestroyInstance(peer);
	return 0;
}

void Server::start(unsigned int maxClients, unsigned int port)
{
	if(m_serverThread == NULL)
	{
		m_maxClients = maxClients;
		m_port = port;

		m_serverThread = MThreadManager::getInstance()->getNewThread();
		m_serverThread->Start(Server::server_thread, "ServerThread", (void*) this);
	}
}
