#ifndef NEO_NATIVETOOLSET_H
#define NEO_NATIVETOOLSET_H

#include <vector>
#include <string>
#include <NeoEngine.h>

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

	virtual std::string listSelection(const std::string& title, const std::vector<std::string>& entries) = 0;

	virtual Neo::Vector4 colorDialog(const Neo::Vector4& start) = 0;
};

#endif //NEO_NATIVETOOLSET_H
