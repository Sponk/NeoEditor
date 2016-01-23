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

#ifndef NEO_PROFILER_H
#define NEO_PROFILER_H

#include <string>
#include <vector>
#include <map>

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif

namespace Neo
{
/**
 * @brief Implements a profiler interface that works on the CPU cycle level of measurement.
 *
 * All measurements are done in CPU cycles to deliver the greatest precision.
 * This also means that this class needs to be customized for every new CPU architecture!
 *
 * Example usage:
 *
 * @code
 * Profiler p;
 *
 * p.functionTimingStart("AwesomeStuff"); // Start recording
 * // Block of code!
 * {
 * 	// Do some awesome stuff!
 * }
 * p.functionTimingStart("AwesomeStuff"); // Stop recording
 * cout << p.generateReport() << endl;
 * @endcode
 */
class NEO_CORE_EXPORT Profiler
{
public:

	/**
	 * @brief Contains the profile of a single block of code or function.
	 */
	struct NEO_CORE_EXPORT FunctionProfile
	{
		unsigned long last; /// The last number of cycles recording was started.
		unsigned long averageTicks; /// Average number of cycles needed for execution.
		unsigned long numberOfCalls; /// Number of calls to this function or block of code

		FunctionProfile() : last(0), averageTicks(0), numberOfCalls(0) {}
	};

	Profiler();

	/**
	 * @brief Returns the number of CPU cycles since bootup.
	 *
	 * This method is highly architecture dependent and currently
	 * only available on IA32 and AMD64 CPUs.
	 *
	 * @return The number ot cycles.
	 */
	unsigned long getTicks()
	{
#if !defined(_MSC_VER) && (defined(__amd64__) || defined(_M_AMD64))
		unsigned long tick;
		asm volatile ( "rdtsc" : "=A"(tick) );
		return tick;
#elif !defined(_MSC_VER) && (defined(i386) || defined(_M_IX86))
		unsigned long tick;
		asm volatile ("rdtsc" : "=A"(tick));
		return tick;
#elif defined(_MSC_VER)
		return __rdtsc();
#else
		return 0;
#endif
	}

	/**
	 * @brief Starts recording the time of a block of code.
	 * @param name The name of the block to record.
	 */
	void functionTimingStart(const char *name)
	{
		FunctionProfile * f = &m_profiles[name];
		f->last = getTicks();
	}

	/**
	 * @brief Finishes recording the time of a block of code.
	 *
	 * This method also calculates some averages to smooth out the result.
	 * @param name The name of the block.
	 */
	void functionTimingEnd(const char *name)
	{
		unsigned long t = getTicks();
		FunctionProfile * f = &m_profiles[name];
		f->averageTicks = (f->averageTicks + (t - f->last)) >> 1;
		f->numberOfCalls++;
	}

	/**
	 * @brief Retrieves the function profile for the given block name.
	 * @param name The block name.
	 * @return The function profile
	 */
	FunctionProfile & getFunctionProfile(const char *name)
	{
		return m_profiles[name];
	}

	String generateReport();

private:
	std::map<std::string, FunctionProfile> m_profiles;
};
}

#endif //NEO_PROFILER_H
