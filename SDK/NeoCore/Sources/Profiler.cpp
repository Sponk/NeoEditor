//========================================================================
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <NeoCore.h>
#include <sstream>

Neo::Profiler::Profiler()
{

}

const char* Neo::Profiler::generateReport()
{
	std::stringstream str;

	//str << "Function\t| Calls\t| Average Cycles" << std::endl;
	for (auto f : m_profiles)
	{
		// str << f.first << "\t\t| " << f.second.numberOfCalls << "\t\t| " << f.second.averageTicks << std::endl;
		//str << f.first << " got " << f.second.numberOfCalls << " calls with " << f.second.averageTicks << " cycles" << std::endl;
		str << f.first << " ==> calls: " << f.second.numberOfCalls << " cycles: " << f.second.averageTicks << std::endl;
	}

	return str.str().c_str();
}
