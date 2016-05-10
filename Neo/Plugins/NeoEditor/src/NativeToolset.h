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
};

#endif //NEO_NATIVETOOLSET_H
