#ifndef __NETWORK_UTILS_H__
#define __NETOWRK_UTILS_H__

#include <BitStream.h>
#include <MCore.h>

void writeString(RakNet::BitStream* bs, const char* str);
void writeVector3(RakNet::BitStream* bs, MVector3 vec);

#endif
