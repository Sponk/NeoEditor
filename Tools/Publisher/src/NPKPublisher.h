#ifndef NEO_NPKPUBLISHER_H
#define NEO_NPKPUBLISHER_H

#include <NeoEngine.h>
#include "Publisher.h"

#ifdef WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

namespace Publish
{

class NPKPublisher : public Publisher
{
protected:
	bool packageDir(const char* dir, const char* pwd, Neo::PackageManager* pkm, Neo::Package pkg, bool verbose = false);
	bool copyFiles(const char* src, const char* dest, const char* type = "", bool verbose = false);
	bool createPackage(const char* src, const char* dest, const char* project, bool verbose = false);
	
public:

	virtual const char* getName() override
	{
		return "NPK";
	}

	virtual bool publish(const char* projectFile,
						 const char* executable,
						 const char* output,
						 bool verbose = false,
						 const char* levelOverride = nullptr,
						 std::function<void(int)> progressCallback = nullptr) override;
};

}

#endif //NEO_NPKPUBLISHER_H
