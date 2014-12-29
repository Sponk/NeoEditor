#include <NetworkUtils.h>

using namespace Neo;
using namespace RakNet;

void writeString(RakNet::BitStream* bs, const char* str)
{
	bs->Write(M_VARIABLE_STRING);
	bs->Write(str);
}

void writeVector3(RakNet::BitStream* bs, MVector3 vec)
{
	bs->WriteVector(vec.x, vec.y, vec.z);
}

NeoVariable readNextArgument(RakNet::BitStream* bs)
{
	int type;
	bs->Read(type);
	switch(type)
	{
		case M_VARIABLE_STRING:
			{
				RakString str;
				bs->IgnoreBytes(sizeof(M_VARIABLE_TYPE));
				bs->Read(str);

				MString* variableString = new MString;
				variableString->set(str.C_String());

				NeoVariable variable("", variableString, M_VARIABLE_STRING);
				return variable;
			}
			break;

		default:
			MLOG_WARNING("Found unknown variable type: " << type);
	}
}

void writeVariable(RakNet::BitStream* bs, Neo::NeoVariable var)
{
	switch(var.getType())
	{
		case M_VARIABLE_STRING:
				writeString(bs, ((MString*) var.getPointer())->getSafeString());
			break;
	}
}

void registerNetworkingScriptAPI(MScriptContext* script)
{

}