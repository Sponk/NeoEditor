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
