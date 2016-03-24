#ifndef NEO_TOOL_H
#define NEO_TOOL_H

#include <string>

struct ProcessPipe
{
	int in = -1;
	int out = -1;
};

class Tool
{
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
		return executeToolBlocking(name, NULL);
	}

	static ProcessPipe executeTool(const char* name, const char* input);
};


#endif //NEO_TOOL_H
