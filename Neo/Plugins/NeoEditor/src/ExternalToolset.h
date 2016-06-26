#ifndef NEO_QTTOOLSET_H
#define NEO_QTTOOLSET_H

#include "NativeToolset.h"

class ExternalToolset: public NativeToolset
{
public:

	virtual std::string fileDialog(const char* message,
								   const char* start,
								   const char* filetypes,
								   bool save,
								   bool directories);


	virtual void messagebox(const char* title, const char* message);
	virtual void aboutDialog();
	virtual Neo::Vector4 colorDialog(const Neo::Vector4& start);
	virtual std::string listSelection(const std::string& title, const std::vector<std::string>& entries);
};

#endif //NEO_QTTOOLSET_H
