#include <Client.h>
#include <MessageIdentifiers.h>
#include <Messenger.h>
#include <BitStream.h>
#include <RakNetTypes.h>
#include <NetworkUtils.h>
#include <vector>
#include <string>

using namespace std;
using namespace Neo;
using namespace RakNet;

int Client::client_thread(void *data)
{
	Client* client = (Client*) data;
	NeoEngine* engine = NeoEngine::getInstance();
	NeoGame* game = engine->getGame();

	Messenger* messenger = Messenger::getInstance();
	messenger->addInbox("ClientThread", 15);

	if(!client)
		return -1;

	MLOG_INFO("Starting network client thread");

	RakPeerInterface* peer = client->m_peer = RakPeerInterface::GetInstance();

	SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	peer->Connect(client->m_host.getSafeString(), client->m_port, 0, 0);

	SystemAddress serverAddress;

	Packet* packet;
	Message msg;
	vector<Message> requests;

	bool connected = false;

	while(game->isRunning())
	{
		while(messenger->getMessagesCount("ClientThread") > 0)
		{
			msg = messenger->getNextMessage("ClientThread");
			requests.push_back(msg);

			if(connected)
			{
				MLOG_INFO("Sending " << msg.message << " to " << serverAddress.ToString());

				BitStream out;
				out.Write((MessageID) msg.messageId);
				writeString(&out, msg.message.c_str());

				std::vector<NeoVariable>* variables = (std::vector<NeoVariable>*) msg.data;
				if(variables)
				{
					for(int i = 0; i < variables->size(); i++)
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
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
						MLOG_INFO("New connection arrived!");
					break;

				case ID_CONNECTION_REQUEST_ACCEPTED:
						MLOG_INFO("Client connection accepted.");
						serverAddress = packet->systemAddress;
						connected = true;
					break;

				case ID_NO_FREE_INCOMING_CONNECTIONS:
						MLOG_INFO("Could not connect to server: Server full!");
						RakPeerInterface::DestroyInstance(peer);
						return 0;
					break;

				// RPC result
				case 1001:
						MLOG_INFO("RPC-Result!");
					break;
			}
		}
	}

	RakPeerInterface::DestroyInstance(peer);
	return 0;
}

void Client::start(const char* host, unsigned int port)
{
	if(m_clientThread == NULL)
	{
		m_port = port;
		m_host.set(host);

		m_clientThread = MThreadManager::getInstance()->getNewThread();
		m_clientThread->Start(Client::client_thread, "ClientThread", (void*) this);
	}
}
