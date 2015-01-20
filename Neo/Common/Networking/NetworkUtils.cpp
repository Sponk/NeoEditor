//========================================================================
// Copyright (c) 2014-2015 Yannick Pflanzer <neo-engine.de>
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
    // MLOG_INFO("Registering networking API...");
}
