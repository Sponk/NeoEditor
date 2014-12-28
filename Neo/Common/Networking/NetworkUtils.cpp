#include <NetworkUtils.h>

void writeString(RakNet::BitStream* bs, const char* str)
{
	bs->Write(str);
}

void writeVector3(RakNet::BitStream* bs, MVector3 vec)
{
	bs->WriteVector(vec.x, vec.y, vec.z);
}
