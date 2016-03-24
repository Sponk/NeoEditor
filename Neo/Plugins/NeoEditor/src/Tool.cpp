#include "Tool.h"
#include <unistd.h>
#include <cstring>
#include <sstream>

ProcessPipe Tool::executeTool(const char* name, const char* input)
{
	int fdParentChild[2];
	int fdChildParent[2];

	pipe(fdParentChild);
	pipe(fdChildParent);

	pid_t pid = fork();
	if(pid == -1)
		return ProcessPipe();

	// Child
	if(!pid)
	{
		close(fdParentChild[1]);
		close(fdChildParent[0]);

		dup2(fdParentChild[0], STDIN_FILENO);
		dup2(fdChildParent[1], STDOUT_FILENO);

		char* const argv[] = { (char*) name, NULL };
		execv(name, argv);
	}
	else // Parent
	{
		close(fdParentChild[0]);
		close(fdChildParent[1]);
	}

	ProcessPipe p;
	p.in = fdChildParent[0];
	p.out = fdParentChild[1];

	if(input)
		write(p.out, input, strlen(input));

	wait();
	return p;
}

std::string Tool::executeToolBlocking(const char* name, const char* input)
{
	std::stringstream ss;
	ProcessPipe p = executeTool(name, input);

	char* buffer = new char[256];
	while(read(p.in, buffer, 255))
	{
		// Make sure the buffer is nul-terminated
		buffer[255] = 0;
		ss << buffer;
	}

	delete[] buffer;
	return ss.str();
}