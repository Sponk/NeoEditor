#ifndef NEO_NPKWEBPUBLISHER_H
#define NEO_NPKWEBPUBLISHER_H

#include <NeoEngine.h>
#include "NPKPublisher.h"

#ifdef WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

namespace Publish
{

class NPKWebPublisher : public NPKPublisher
{
public:

	virtual const char* getName() override
	{
		return "Web NPK";
	}

	virtual bool publish(const char* projectFile,
				const char* executable,
				const char* output,
				bool verbose = false,
				const char* levelOverride = nullptr,
				std::function<void(int)> progressCallback = nullptr) override;
};

}

#endif //NEO_NPKWEBPUBLISHER_H
