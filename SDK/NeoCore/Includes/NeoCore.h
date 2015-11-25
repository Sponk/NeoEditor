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


#ifndef __CORE_H
#define __CORE_H

#ifdef WIN32
	#ifdef _MSC_VER
		#pragma warning(disable: 4251)
	#endif

	#if defined(NEO_CORE_DLL)
		#define NEO_CORE_EXPORT __declspec( dllexport )
	#elif defined(NEO_CORE_STATIC)
		#define NEO_CORE_EXPORT
	#else
		#define NEO_CORE_EXPORT __declspec( dllimport )
	#endif

#else
	#define NEO_CORE_EXPORT
#endif

namespace Neo
{
class Image;
class Sound;
}

#include <Utils.h>
#include <NeoString.h>
#include <File.h>
#include <StdFile.h>
#include <FileTools.h>
#include <Maths.h>
#include <SystemContext.h>
#include <InputContext.h>
#include <RenderingContext.h>
#include <SoundContext.h>
#include <PhysicsContext.h>
#include <ScriptContext.h>
#include <Image.h>
#include <Sound.h>
#include <StringTools.h>
#include <DataManager.h>
#include <DataLoader.h>
#include <Thread.h>
#include <Profiler.h>

#endif
