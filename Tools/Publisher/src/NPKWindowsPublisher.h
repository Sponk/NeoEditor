#ifndef NPKWINDOWSPUBLISHER_H
#define NPKWINDOWSPUBLISHER_H

#include "NPKPublisher.h"

namespace Publish
{

class NPKWindowsPublisher : public NPKPublisher
{
public:
	NPKWindowsPublisher();

	virtual const char* getName() override
	{
		return "WindowsNPK";
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
