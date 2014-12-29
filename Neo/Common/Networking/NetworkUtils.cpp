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
    bs->Write(M_VARIABLE_VEC3);
    bs->Write(vec.x);
    bs->Write(vec.y);
    bs->Write(vec.z);
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
				bs->Read(str);

				MString* variableString = new MString;
				variableString->set(str.C_String());

				NeoVariable variable("", variableString, M_VARIABLE_STRING);
				return variable;
			}
			break;

    case M_VARIABLE_VEC3:
        {
            MVector3* vec = new MVector3;
            bs->Read(vec->x);
            bs->Read(vec->y);
            bs->Read(vec->z);

            NeoVariable variable("", vec, M_VARIABLE_VEC3);
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

        case M_VARIABLE_VEC3:
                writeVector3(bs, *((MVector3*) var.getPointer()));
             break;
	}
}

void registerNetworkingScriptAPI(MScriptContext* script)
{

}
