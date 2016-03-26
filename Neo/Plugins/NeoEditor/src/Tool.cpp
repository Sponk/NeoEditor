#include "Tool.h"
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <NeoEngine.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#ifndef _WIN32
Tool Tool::executeTool(const char* name, const char* input)
{
	int fdParentChild[2];
	int fdChildParent[2];

	pipe(fdParentChild);
	pipe(fdChildParent);

	pid_t pid = fork();
	if(pid == -1)
		return Tool();

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

	Tool t;
	t.in = fdChildParent[0];
	t.out = fdParentChild[1];

	if(input)
		t.write(input, strlen(input));

	return t;
}

size_t Tool::write(const char* buffer, size_t size)
{
	return ::write(out, buffer, size);
}

size_t Tool::read(char* buffer, size_t size)
{
	return ::read(in, buffer, size);
}

#else

Tool Tool::executeTool(const char* name, const char* input)
{
	MLOG_INFO("Executing " << name);
	Tool t;

	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE stdoutWr = 0, stdinRd = 0;

	// STDOUT
	if (!CreatePipe(&t.in, &stdoutWr, &saAttr, 0))
	{
		MLOG_ERROR("Could not open pipe!");
		return Tool();
	}

	if (!SetHandleInformation(t.in, HANDLE_FLAG_INHERIT, 0))
	{
		MLOG_ERROR("Could not set handle information!");
		return Tool();
	}

	// STDIN
	if (!CreatePipe(&stdinRd, &t.out, &saAttr, 0))
	{
		MLOG_ERROR("Could not open pipe!");
		return Tool();
	}

	if (!SetHandleInformation(t.out, HANDLE_FLAG_INHERIT, 0))
	{
		MLOG_ERROR("Could not set handle information!");
		return Tool();
	}

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdOutput = stdoutWr;
	siStartInfo.hStdInput = stdinRd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	BOOL success = CreateProcess(NULL, (LPSTR) name, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

	if (!success)
	{
		MLOG_ERROR("Could not start child!");
		return Tool();
	}

	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);

	CloseHandle(stdinRd);
	CloseHandle(stdoutWr);

	t.m_isrunning = true;
	return t;
}

size_t Tool::write(const char * buffer, size_t size)
{
	return size_t();
}

size_t Tool::read(char * buffer, size_t size)
{
	DWORD rbytes = 0;
	BOOL success = false;
	success = ReadFile(in, buffer, size, &rbytes, NULL);
	return rbytes; // (success) ? rbytes : 0;
}

#endif

std::string Tool::executeToolBlocking(const char* name, const char* input)
{
	std::stringstream ss;
	Tool t = Tool::executeTool(name, input);

	if (!t.isRunning())
		return std::string();

	char* buffer = new char[256];
	while (t.read(buffer, 255))
	{
		// Make sure the buffer is nul-terminated
		buffer[255] = 0;
		ss << buffer;
	}

	delete[] buffer;
	return ss.str();
}

