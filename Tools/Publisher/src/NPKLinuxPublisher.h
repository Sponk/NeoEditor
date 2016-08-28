#ifndef NPKLINUXPUBLISHER_H
#define NPKLINUXPUBLISHER_H

#include "NPKPublisher.h"

namespace Publish
{

class NPKLinuxPublisher : public NPKPublisher
{
public:
	NPKLinuxPublisher();

	virtual const char* getName() override
	{
		return "LinuxNPK";
	}

	virtual bool publish(const char* projectFile,
						 const char* executable,
						 const char* output,
						 bool verbose = false,
						 const char* levelOverride = nullptr,
						 std::function<void(int)> progressCallback = nullptr) override;
};

}

#endif // NPKLINUXPUBLISHER_H
