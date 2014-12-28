#include <NetworkUtils.h>

using namespace Neo;
using namespace RakNet;

void writeString(RakNet::BitStream* bs, const char* str)
{
	bs->Write(ID_STRING);
	bs->Write(str);
}

void writeVector3(RakNet::BitStream* bs, MVector3 vec)
{
	bs->WriteVector(vec.x, vec.y, vec.z);
}

NeoVariable readNextArgument(RakNet::BitStream* bs)
{
	switch(bs->GetData()[bs->GetReadOffset()/8])
	{
		case ID_STRING:
			{
				RakString str;
				bs->IgnoreBytes(sizeof(ARGUMENT_TYPE));
				bs->Read(str);

				MString* variableString = new MString;
				variableString->set(str.C_String());

				NeoVariable variable("", variableString, M_VARIABLE_STRING);
				return variable;
			}
			break;
	}
}

void writeVariable(RakNet::BitStream* bs, Neo::NeoVariable var)
{
	switch(var.getType())
	{
		case M_VARIABLE_STRING:
				MLOG_INFO("Found string");
				writeString(bs, ((MString*) var.getPointer())->getSafeString());
			break;
	}
}
