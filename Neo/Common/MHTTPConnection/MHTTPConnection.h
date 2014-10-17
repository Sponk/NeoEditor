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

#if !defined(MHTTP_CONNECTION_H) && defined(USE_NETWORKING)
#define MHTTP_CONNECTION_H

#include <TCPInterface.h>
#include <HTTPConnection.h>
#include <MEngine.h>

/**
 * The MHTTPConnection class allows to very easily send either POST requests
 * or GET requests to the server it was initialized with.
 */
class MHTTPConnection
{
    RakNet::TCPInterface m_tcp;
    RakNet::HTTPConnection m_connection;

    bool m_init;
    MString m_host;

    const char* connectionLoop();

public:
    /**
     * @brief The constructor establishes a connection to host.
     * @param host The URL or IP of the host
     * @param port The port to connect to (usually 80)
     */
    MHTTPConnection(const char* host, int port);

    /**
     * @brief Sends a post request to the connected server.
     *
     * If you want to access your data from PHP using the $_POST array
     * you will need to give your data the form of "var1=value1?var2=value2"
     *
     * @param path The file to request (for example "/login.php"). Needs to be an absolute path!
     * @param data Your data string you want to send.
     * @return Returns a pointer to the servers response or NULL if it failed. The string needs to be free'd!
     */
    const char* sendPostRequest(const char* path, const char* data);

    /**
     * @brief Sends a get request to the connected server.
     *
     * @param path The file to request (for example "/login.php"). Needs to be an absolute path!
     * @return Returns a pointer to the servers response or NULL if it failed. The string needs to be free'd!
     */
    const char* sendGetRequest(const char* path);
};

#endif
