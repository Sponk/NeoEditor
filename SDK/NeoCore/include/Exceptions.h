#ifndef NEO_EXCEPTIONS_H
#define NEO_EXCEPTIONS_H

#include <exception>

namespace Neo
{
	class NeoFileNotFound : std::exception
	{
		String filename;
	public:
		NeoFileNotFound(const char* file) : filename(file) {}
		virtual const char* what() { return filename.getSafeString(); }
	};
}

#endif //NEO_EXCEPTIONS_H
