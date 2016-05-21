#ifndef NEO_NATIVETOOLSET_H
#define NEO_NATIVETOOLSET_H

#include <string>

class NativeToolset
{
public:
	virtual std::string fileDialog(const char* message, const char* start, const char* filetypes, bool save, bool directories) = 0;
	virtual std::string fileOpenDialog(const char* message, const char* start, const char* filetypes)
	{
		return this->fileDialog(message, start, filetypes, false, false);
	}

	virtual std::string fileSaveDialog(const char* message, const char* start, const char* filetypes)
	{
		return this->fileDialog(message, start, filetypes, true, false);
	}

	virtual std::string directoryOpenDialog(const char* message, const char* start)
	{
		return this->fileDialog(message, start, "", false, true);
	}
	
	virtual void messagebox(const char* title, const char* message) = 0;
	virtual void aboutDialog() = 0;
};

#endif //NEO_NATIVETOOLSET_H
