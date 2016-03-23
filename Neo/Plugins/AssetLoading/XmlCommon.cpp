//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#include <NeoEngine.h>
#include <tinyxml.h>
#include "XmlCommon.h"

namespace Neo
{

void openNode(File * file, const char * name, unsigned int tab, const char * sep)
{
	for(unsigned int i=0; i<tab; i++)
		M_fprintf(file, "\t");
	M_fprintf(file, "<%s>", name);
	if(sep)
		M_fprintf(file, sep);
}

void closeNode(File * file, const char * name, unsigned int tab, const char * sep)
{
	for(unsigned int i=0; i<tab; i++)
		M_fprintf(file, "\t");
	M_fprintf(file, "</%s>", name);
	if(sep)
		M_fprintf(file, sep);
}

void openAttributeNode(File * file, const char * name, unsigned int tab)
{
	for(unsigned int i=0; i<tab; i++)
		M_fprintf(file, "\t");
	M_fprintf(file, "<%s ", name);
}

void closeAttributeNode(File * file, const char * sep)
{
	M_fprintf(file, " />");
	if(sep)
		M_fprintf(file, sep);
}

void closeNodeAttributes(File * file, const char * sep)
{
	M_fprintf(file, ">");
	if(sep)
		M_fprintf(file, sep);
}

void writeBool(File * file, const char * name, bool variable, const char * sep)
{
	M_fprintf(file, "%s=\"", name);
	if(variable)
		M_fprintf(file, "true");
	else
		M_fprintf(file, "false");
	M_fprintf(file, "\"");
	if(sep)
		M_fprintf(file, sep);
}

void writeString(File * file, const char * name, const char * string, const char * sep)
{
	TIXML_STRING S1(string);
	TIXML_STRING S2;
	TiXmlBase::EncodeString(S1, &S2);

	M_fprintf(file, "%s=\"%s\"", name, S2.c_str());
	if(sep)
		M_fprintf(file, sep);
}

void writeInt(File * file, const char * name, int variable, const char * sep)
{
	M_fprintf(file, "%s=\"%d\"", name, variable);
	if(sep)
		M_fprintf(file, sep);
}

void writeFloat(File * file, const char * name, float variable, const char * sep)
{
	M_fprintf(file, "%s=\"%f\"", name, variable);
	if(sep)
		M_fprintf(file, sep);
}

void writeFloatValues(File * file, const char * name, float * vector, unsigned int size, const char * sep)
{
	M_fprintf(file, "%s=\"", name);
	for(unsigned int i=0; i<size; i++)
	{
		if(i>0) M_fprintf(file, " ");
		M_fprintf(file, "%f", *vector);
		vector++;
	}
	M_fprintf(file, "\"");
	if(sep)
		M_fprintf(file, sep);
}

void writeFilename(File * file, const char * name, const char * filename, const char * workingDirectory, const char * sep)
{
	if(workingDirectory)
	{
		char localFilename[256];
		getLocalFilename(localFilename, workingDirectory, filename);
		writeString(file, name, localFilename);
	}
	else
	{
		writeString(file, name, filename);
	}
	
	if(sep)
		M_fprintf(file, sep);
}

void writeVariable(File * file, NeoVariable * variable, const char * workingDirectory, const char * sep)
{
    const char * name = variable->getName();
    
    switch(variable->getType())
    {
		case M_VARIABLE_BOOL:
			writeBool(file, name, *((bool*)variable->getPointer()));
			break;
		case M_VARIABLE_INT:
		case M_VARIABLE_UINT:
			writeInt(file, name, *((int*)variable->getPointer()));
			break;
		case M_VARIABLE_FLOAT:
			writeFloat(file, name, *((float*)variable->getPointer()));
			break;
		case M_VARIABLE_STRING:
			writeString(file, name, ((String*)variable->getPointer())->getSafeString());
			break;
		case M_VARIABLE_VEC2:
			writeFloatValues(file, name, *((Vector2*)variable->getPointer()), 2);
			break;
		case M_VARIABLE_VEC3:
			writeFloatValues(file, name, *((Vector3*)variable->getPointer()), 3);
			break;
		case M_VARIABLE_VEC4:
			writeFloatValues(file, name, *((Vector4*)variable->getPointer()), 4);
			break;
		case M_VARIABLE_TEXTURE_REF:
        {
            TextureRef * textureRef = *(TextureRef **)variable->getPointer();
            if(textureRef)
				writeFilename(file, name, textureRef->getFilename(), workingDirectory);
            break;
        }
    }
	
	if(sep)
		M_fprintf(file, sep);
}
}
