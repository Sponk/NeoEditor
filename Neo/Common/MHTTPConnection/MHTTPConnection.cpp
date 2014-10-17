//========================================================================
// Copyright (c) 2014 Yannick Pflanzer
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

#ifdef USE_NETWORKING
#include "MHTTPConnection.h"
#include <RakSleep.h>

MHTTPConnection::MHTTPConnection(const char* host, int port)
{
    if(!m_tcp.Start(0, 1))
    {
        MLOG_ERROR("Could not start TCP server!");
        m_init = false;
        return;
    }

    m_connection.Init(&m_tcp, host, 80);

    m_init = true;
    m_host.set(host);
}

const char* MHTTPConnection::sendPostRequest(const char* path, const char* data)
{
    m_connection.Post(path, data, "text/html");
    return connectionLoop();
}

const char* MHTTPConnection::sendGetRequest(const char* path)
{
    m_connection.Get(path);
    return connectionLoop();
}

const char* MHTTPConnection::connectionLoop()
{
    while(true)
    {
        RakNet::Packet *packet = m_tcp.Receive();
        if(packet)
        {
            m_connection.ProcessTCPPacket(packet);
            m_tcp.DeallocatePacket(packet);
        }

        m_connection.Update();

        if (m_connection.IsBusy() == false)
        {
            RakNet::RakString content = m_connection.Read();
            if(content.GetLength() > 0)
            {
                char* ret = new char[content.GetLength()];
                strcpy(ret, content.C_String());
                return ret;
            }
            else
            {
                return NULL;
            }
        }

        RakSleep(30);
    }
}
#endif
