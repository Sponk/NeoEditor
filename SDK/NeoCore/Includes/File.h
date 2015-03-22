//========================================================================
// Copyright (c) 2012 Philipp Geyer <http://nistur.com>
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


#ifndef __FILE_H
#define __FILE_H

namespace Neo
{
/// File I/O class. This interface can be used manually, but will usually be
/// used from the FileTools interface, to more closely match standard C file I/O.
class NEO_CORE_EXPORT File
{
public:
	
	/// Destructor.
	virtual ~File(void){}
	
	/// Opens a file stream with the name specified by path. This usually should 
	/// not be used directly and should be opened from the MFileTools interface.
	/// \param path		Path to file
	/// \param mode		Read or write mode ('r' or 'w')
	virtual void	open(const char * path, const char * mode) = 0;
	
	/// Close a file stream, if open.
	virtual int		close() = 0;
	
	/// Reads count amount of elements of data, each size bytes long, from the file, 
	/// storing them at the location given by dest. Returns the number of items 
	/// successfully read. If an error occurs, or the end-of-file is reached, the return 
	/// value is a short item count (or zero).
	/// \param dest		Data
	/// \param size		Data element size in bytes
	/// \param count	Number of elements to read
	/// \return		Number of elements read
	virtual size_t	read(void * dest, size_t size, size_t count) = 0;
	
	/// Writes count amount of elements of data, each size bytes long, to the file, 
	/// obtaining them from the location given by str. Returns the number of items 
	/// successfully written.
	/// \param str		Data
	/// \param size		Data element size in bytes
	/// \param count	Number of elements to write
	/// \return		Number of elements written
	virtual size_t	write(const void * str, size_t size, size_t count) = 0;
	
	/// Produces output according to the standard printf() style format and writes to 
	/// the file. Returns the number of characters written to the file (excluding the 
	/// null byte used to end output strings).
	/// \param format	Format string
	/// \return		Number of characters written
	virtual int		print(const char * format, ...) = 0;

	/// Produces output according to the standard printf() style format and writes to 
	/// the file. Returns the number of characters written to the file (excluding the 
	/// null byte used to end output strings).
	/// \param format	Format string
	/// \param args		Arguments
	/// \return 		Number of characters written
	virtual int		print(const char * format, va_list args) = 0;
	
	/// Sets the file position indicator. The new position, measured in bytes, is obtained
	/// by adding offset bytes to the position specified by whence. If whence is set to 
	/// SEEK_SET, SEEK_CUR, or SEEK_END, the offset is relative to the start of the file, 
	/// the current position indicator, or end-of-file respectively.
	/// \param offset	Offset position in bytes
	/// \param whence	SEEK_SET, SEEK_CUR, or SEEK_END
	/// \return		The resulting location
	virtual int		seek(long offset, int whence) = 0;
	
	/// Obtains the current value of the file position indicator for the file.
	/// \return		Current file position
	virtual long	tell() = 0;
	
	/// Sets the file position indicator to the beginning of the file.
	virtual void	rewind() = 0;
	
	/// Checks if file is open.
	/// \return	True if file is open.
	virtual bool 	isOpen() = 0;
	
	/// Destroy file stream.
	virtual void	destroy() = 0;
};

// File Open Hook
class NEO_CORE_EXPORT FileOpenHook
{
public:
	virtual ~FileOpenHook(void){}
	virtual File * open(const char * path, const char * mode) = 0;
};
}
#endif
