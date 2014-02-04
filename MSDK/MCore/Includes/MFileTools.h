/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// FileTools.h
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
// jan 2012, FILE wrapper by Philipp Geyer <http://nistur.com>


#ifndef _FILE_TOOLS_H
#define _FILE_TOOLS_H

/// Collection of file manipulation tools, including File I/O designed to 
/// directly replace stdio, using MFile* in place of FILE*.

// general file and directory tools
M_CORE_EXPORT bool copyFile(const char * inFilename, const char * outFilename);
M_CORE_EXPORT bool createDirectory(const char * filename, bool recursive = false);
M_CORE_EXPORT bool isDirectory(const char * filename);
M_CORE_EXPORT bool isEmptyDirectory(const char * filename);
M_CORE_EXPORT bool clearDirectory(const char * filename);
M_CORE_EXPORT bool removeDirectory(const char * filename);
M_CORE_EXPORT bool isFileExist(const char * filename);
M_CORE_EXPORT bool copyDirectory(const char * inFilename, const char * outFilename);
M_CORE_EXPORT bool readDirectory(const char * filename, vector<string> * files, bool hiddenFiles = false, bool recursive = false);

// file wrapper
M_CORE_EXPORT void M_registerFileOpenHook(MFileOpenHook * hook);
M_CORE_EXPORT MFileOpenHook * M_getFileOpenHook();

/// Opens a file stream with the name specified by path.
/// \param path		Path to file
/// \param mode		Read or write mode ('r' or 'w')
M_CORE_EXPORT MFile* M_fopen(const char * path, const char* mode);

/// Close a file stream, if open.
/// \param stream	File stream
M_CORE_EXPORT int	 M_fclose(MFile * stream);

/// Reads count amount of elements of data, each size bytes long, from the file, 
/// storing them at the location given by dest. Returns the number of items 
/// successfully read. If an error occurs, or the end-of-file is reached, the return 
/// value is a short item count (or zero).
/// \param dest		Data
/// \param size		Data element size in bytes
/// \param count	Number of elements to read
/// \param stream	File stream
/// \return		Number of elements read
M_CORE_EXPORT size_t M_fread(void * dest, size_t size, size_t count, MFile * stream);

/// Writes count amount of elements of data, each size bytes long, to the file, 
/// obtaining them from the location given by str. Returns the number of items 
/// successfully written.
/// \param str		Data
/// \param size		Data element size in bytes
/// \param count	Number of elements to write
/// \param stream	File stream
/// \return		Number of elements written
M_CORE_EXPORT size_t M_fwrite(const void * str, size_t size, size_t count, MFile * stream);

/// Produces output according to the standard printf() style format and writes to 
/// the file. Returns the number of characters written to the file (excluding the 
/// null byte used to end output strings).
/// \param format	Format string
/// \return		Number of characters written
M_CORE_EXPORT int	 M_fprintf(MFile * stream, const char * format, ...);

/// Sets the file position indicator. The new position, measured in bytes, is obtained
/// by adding offset bytes to the position specified by whence. If whence is set to 
/// SEEK_SET, SEEK_CUR, or SEEK_END, the offset is relative to the start of the file, 
/// the current position indicator, or end-of-file respectively.
/// \param offset	Offset position in bytes
/// \param whence	SEEK_SET, SEEK_CUR, or SEEK_END
/// \return		The resulting location
M_CORE_EXPORT int	 M_fseek(MFile * stream, long offset, int whence);

/// Obtains the current value of the file position indicator for the file.
/// \return		Current file position
M_CORE_EXPORT long	 M_ftell(MFile * stream);

/// Sets the file position indicator to the beginning of the file.
M_CORE_EXPORT void	 M_rewind(MFile * stream);

#endif