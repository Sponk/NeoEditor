#ifndef NEO_TOOL_H
#define NEO_TOOL_H

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

class Tool
{
#ifdef _WIN32
	HANDLE in;
	HANDLE out;
#else
	int in = -1;
	int out = -1;
#endif

	bool m_isrunning = false;
public:

	/**
	 * @brief Executes the given tool, writes the options to its stdin and reads the result from the child's stdout.
	 *
	 * @param name The name of the tool.
	 * @param input The input parameters for the tool.
	 */
	static std::string executeToolBlocking(const char* name, const char* input);
	static std::string executeToolBlocking(const char* name)
	{
		return Tool::executeToolBlocking(name, NULL);
	}

	static std::string executeToolNonBlocking(const char* name, const char* input);
	static std::string executeToolNonBlocking(const char* name)
	{
		return Tool::executeToolNonBlocking(name, NULL);
	}

	static Tool executeTool(const char* name, const char* input);

	size_t read(char* buffer, size_t size);
	size_t write(const char* buffer, size_t size);

	bool isRunning() { return m_isrunning; }
};


#endif //NEO_TOOL_H
