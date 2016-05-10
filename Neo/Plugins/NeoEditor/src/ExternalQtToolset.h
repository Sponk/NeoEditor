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
};

#endif //NEO_QTTOOLSET_H
