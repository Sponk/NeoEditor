#ifndef __NETWORK_UTILS_H__
#define __NETOWRK_UTILS_H__

#include <BitStream.h>
#include <MCore.h>
#include <NeoEngine.h>

enum ARGUMENT_TYPE
{
	ID_STRING = 0,
	ID_VECTOR3
};

struct RpcArgumentList
{
	unsigned int size;
	Neo::NeoVariable* variables[];
};

void writeString(RakNet::BitStream* bs, const char* str);
void writeVector3(RakNet::BitStream* bs, MVector3 vec);

void writeVariable(RakNet::BitStream* bs, Neo::NeoVariable var);

Neo::NeoVariable readNextArgument(RakNet::BitStream* bs);

void registerNetworkingScriptAPI(MScriptContext* script);

#endif
