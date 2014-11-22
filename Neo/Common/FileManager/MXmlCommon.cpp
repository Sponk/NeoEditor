/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MXmlCommon.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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


#include <MEngine.h>
#include <tinyxml.h>
#include "MXmlCommon.h"


void openNode(MFile * file, const char * name, unsigned int tab, const char * sep)
{
	for(unsigned int i=0; i<tab; i++)
		M_fprintf(file, "\t");
	M_fprintf(file, "<%s>", name);
	if(sep)
		M_fprintf(file, sep);
}

void closeNode(MFile * file, const char * name, unsigned int tab, const char * sep)
{
	for(unsigned int i=0; i<tab; i++)
		M_fprintf(file, "\t");
	M_fprintf(file, "</%s>", name);
	if(sep)
		M_fprintf(file, sep);
}

void openAttributeNode(MFile * file, const char * name, unsigned int tab)
{
	for(unsigned int i=0; i<tab; i++)
		M_fprintf(file, "\t");
	M_fprintf(file, "<%s ", name);
}

void closeAttributeNode(MFile * file, const char * sep)
{
	M_fprintf(file, " />");
	if(sep)
		M_fprintf(file, sep);
}

void closeNodeAttributes(MFile * file, const char * sep)
{
	M_fprintf(file, ">");
	if(sep)
		M_fprintf(file, sep);
}

void writeBool(MFile * file, const char * name, bool variable, const char * sep)
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

void writeString(MFile * file, const char * name, const char * string, const char * sep)
{
	TIXML_STRING S1(string);
	TIXML_STRING S2;
	TiXmlBase::EncodeString(S1, &S2);

	M_fprintf(file, "%s=\"%s\"", name, S2.c_str());
	if(sep)
		M_fprintf(file, sep);
}

void writeInt(MFile * file, const char * name, int variable, const char * sep)
{
	M_fprintf(file, "%s=\"%d\"", name, variable);
	if(sep)
		M_fprintf(file, sep);
}

void writeFloat(MFile * file, const char * name, float variable, const char * sep)
{
	M_fprintf(file, "%s=\"%f\"", name, variable);
	if(sep)
		M_fprintf(file, sep);
}

void writeFloatValues(MFile * file, const char * name, float * vector, unsigned int size, const char * sep)
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

void writeFilename(MFile * file, const char * name, const char * filename, const char * workingDirectory, const char * sep)
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

void writeVariable(MFile * file, MVariable * variable, const char * workingDirectory, const char * sep)
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
			writeString(file, name, ((MString*)variable->getPointer())->getSafeString());
			break;
		case M_VARIABLE_VEC2:
			writeFloatValues(file, name, *((MVector2*)variable->getPointer()), 2);
			break;
		case M_VARIABLE_VEC3:
			writeFloatValues(file, name, *((MVector3*)variable->getPointer()), 3);
			break;
		case M_VARIABLE_VEC4:
			writeFloatValues(file, name, *((MVector4*)variable->getPointer()), 4);
			break;
		case M_VARIABLE_TEXTURE_REF:
        {
            MTextureRef * textureRef = *(MTextureRef **)variable->getPointer();
            if(textureRef)
				writeFilename(file, name, textureRef->getFilename(), workingDirectory);
            break;
        }
    }
	
	if(sep)
		M_fprintf(file, sep);
}

