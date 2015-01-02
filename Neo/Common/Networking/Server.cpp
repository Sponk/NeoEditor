#include <Server.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <NetworkUtils.h>

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

	SocketDescriptor sd;
	Messenger* messenger = Messenger::getInstance();
	
	const char* thread_name;
	bool connected = false;

	if (server->m_isServer)
	{
		sd = SocketDescriptor(server->m_port, 0);
		peer->Startup(server->m_maxClients, &sd, 1);
		peer->SetMaximumIncomingConnections(server->m_maxClients);
		messenger->addInbox("ServerThread", 15);
		thread_name = "ServerThread";
		connected = true;
	}
	else
	{
		peer->Startup(1, &sd, 1);
		peer->Connect(server->m_host.getSafeString(), server->m_port, 0, 0);
		messenger->addInbox("ClientThread", 15);
		thread_name = "ClientThread";
	}


	SystemAddress serverAddress;
	Message msg;
	vector<Message> requests;
	Packet* packet;

	while(game->isRunning())
	{
		while (messenger->getMessagesCount(thread_name) > 0)
		{
			msg = messenger->getNextMessage(thread_name);
			requests.push_back(msg);

			if(connected)
			{
				MLOG_INFO("Sending " << msg.message << " to " << serverAddress.ToString());

				BitStream out;
				out.Write((MessageID)msg.messageId);
				writeString(&out, msg.message.c_str());

				std::vector<NeoVariable>* variables = (std::vector<NeoVariable>*) msg.data;
				if (variables)
				{
					for (int i = 0; i < variables->size(); i++)
					{
						writeVariable(&out, variables->at(i));
					}
				}

				peer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
			}
		}

		for(packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
		{
			switch(packet->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
						MLOG_INFO("New client connected: " << packet->systemAddress.ToString(true, ':'));
					break;

				case ID_DISCONNECTION_NOTIFICATION:
						MLOG_INFO("Disconnected from " << packet->systemAddress.ToString(true, ':'));
					break;

				case ID_CONNECTION_LOST:
						MLOG_INFO("Lost connection to " << packet->systemAddress.ToString(true, ':'));
					break;

				case ID_RPC_MESSAGE:
					{
						RakString messageStr;

						BitStream in(packet->data, packet->length, false);
						in.IgnoreBytes(sizeof(MessageID));

						int type;
						in.Read(type);

						switch(type)
						{
							case M_VARIABLE_STRING:
									in.Read(messageStr);
								break;

							default:
								MLOG_WARNING("Found unknown variable type: " << type);
						}

						if(messageStr.IsEmpty())
							break;

						MLOG_INFO("RPC-Request from " << packet->systemAddress.ToString(true, ':'));

						void (*func)(RakNet::BitStream*) = server->m_rpcFunctions[messageStr.C_String()];

						if(func)
							func(&in);
						else
							MLOG_ERROR("Requested call does not exitst!");

					}
					break;

				case ID_CONNECTION_REQUEST_ACCEPTED:
					MLOG_INFO("Client connection accepted.");
					serverAddress = packet->systemAddress;
					connected = true;
					break;

				default:
						MLOG_INFO("Message with identifier " << (int) packet->data[0] << " has arrived.");
			}
		}
	}

	RakPeerInterface::DestroyInstance(peer);
	return 0;
}

void Server::startServer(unsigned int maxClients, unsigned int port)
{
	if(m_serverThread == NULL)
	{
		m_isServer = true;
		m_maxClients = maxClients;
		m_port = port;

		m_serverThread = MThreadManager::getInstance()->getNewThread();
		m_serverThread->Start(Server::server_thread, "ServerThread", (void*) this);
	}
}

void Server::startClient(const char* host, unsigned int port)
{
	if (m_serverThread == NULL)
	{
		m_isServer = false;
		m_port = port;
		m_host.set(host);

		m_serverThread = MThreadManager::getInstance()->getNewThread();
		m_serverThread->Start(Server::server_thread, "ClientThread", (void*) this);
	}
}
