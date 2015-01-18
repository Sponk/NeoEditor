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

#include "HTTPRequest.h"
#include "Downloader.h"

using namespace Neo;

HTTPRequest::HTTPRequest(const char* host, int port)
{
   m_host.set(host);
   m_port = port;
}

const char* HTTPRequest::sendPostRequest(const char* path, const char* data)
{
	return postRequestToString(m_host.getSafeString(), path, data, NULL, m_port).c_str();
}

const char* HTTPRequest::sendGetRequest(const char* path)
{
	return downloadFileToString(m_host.getSafeString(), path, NULL, m_port).c_str();
}

