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

#ifndef __NETWORK_UTILS_H__
#define __NETOWRK_UTILS_H__

#include <BitStream.h>
#include <MCore.h>
#include <NeoEngine.h>

/**
 * @brief Writes a string as argument to the given BitStream
 * @param bs The RakNet::BitStream object.
 * @param str The string to write
 */
void writeString(RakNet::BitStream* bs, const char* str);

/**
 * @brief Writes a Vector3 to the given bitstream.
 * @param bs The RakNet::BitStream object.
 * @param vec The vector to write
 */
void writeVector3(RakNet::BitStream* bs, MVector3 vec);

/**
 * @brief Writes an Neo::NeoVariable object to the given BitStream
 * @param bs The RakNet::BitStream object.
 * @param var The NeoVariable.
 *
 * @see Neo::NeoVariable
 */
void writeVariable(RakNet::BitStream* bs, Neo::NeoVariable var);

/**
 * @brief Reads the next argument from the given BitStream.
 *
 * ATTENTION: The pointer in the returned NeoVariable object has to be
 * delete'd after usage. Do this by issuing:
 *
 * @code
 * Neo::NeoVariable variable = readNextArgument(bitstream);
 *
 * // Do some stuff
 * // ...
 * // Free memory. Replace VariableType by the real type, MString for example.
 * delete (VariableType*) variable.getPointer();
 *
 * @endcode
 *
 * @param bs The RakNet::BitStream object.
 * @return The Neo::NeoVariable object containing the data.
 */
Neo::NeoVariable readNextArgument(RakNet::BitStream* bs);

void registerNetworkingScriptAPI(MScriptContext* script);

#endif
