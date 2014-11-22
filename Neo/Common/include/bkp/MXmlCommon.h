/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MXmlCommon.h
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


#ifndef _M_XML_COMMON_H
#define _M_XML_COMMON_H

void openNode(MFile * file, const char * name, unsigned int tab, const char * sep = NULL);
void closeNode(MFile * file, const char * name, unsigned int tab, const char * sep = NULL);
void openAttributeNode(MFile * file, const char * name, unsigned int tab);
void closeAttributeNode(MFile * file, const char * sep = NULL);
void closeNodeAttributes(MFile * file, const char * sep = NULL);
void writeBool(MFile * file, const char * name, bool variable, const char * sep = NULL);
void writeString(MFile * file, const char * name, const char * string, const char * sep = NULL);
void writeInt(MFile * file, const char * name, int variable, const char * sep = NULL);
void writeFloat(MFile * file, const char * name, float variable, const char * sep = NULL);
void writeFloatValues(MFile * file, const char * name, float * vector, unsigned int size, const char * sep = NULL);
void writeFilename(MFile * file, const char * name, const char * filename, const char * workingDirectory = NULL, const char * sep = NULL);
void writeVariable(MFile * file, MVariable * variable, const char * workingDirectory = NULL, const char * sep = NULL);

#endif