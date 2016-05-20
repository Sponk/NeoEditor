#ifndef NEO_QTTOOLSET_H
#define NEO_QTTOOLSET_H

#include "NativeToolset.h"

class ExternalQtToolset : public NativeToolset
{
public:

	virtual std::string fileDialog(const char* message,
								   const char* start,
								   const char* filetypes,
								   bool save,
								   bool directories);


	virtual void messagebox(const char* title, const char* message);
};

#endif //NEO_QTTOOLSET_H
