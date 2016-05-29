#include "Tool.h"
#include <cstring>
#include <sstream>
#include <fcntl.h>
#include <NeoEngine.h>
#include <thread>
#include <atomic>

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
	t.m_isrunning = true;

	if(input)
		t.write(input, strlen(input));

	close(t.out);
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
	Tool t;

	/*char buf[256];
	GetCurrentDirectory(sizeof(buf), buf);
	std::string exename(buf);
	exename += "\\";
	exename += name;
	exename += ".exe";*/
	MLOG_INFO("Executing " << name);

	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	HANDLE stdoutWr = 0, stdinRd = 0;

	t.write(input, strlen(input));

	// Create a pipe for the child process's STDOUT.

	if (! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
	{
		MLOG_ERROR("Could not create pipe!");
		return Tool();
	}

// Ensure the read handle to the pipe for STDOUT is not inherited.

	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
	{
		MLOG_ERROR("Could not create pipe!");
		return Tool();
	}

// Create a pipe for the child process's STDIN.

	if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
	{
		MLOG_ERROR("Could not create pipe!");
		return Tool();
	}

// Ensure the write handle to the pipe for STDIN is not inherited.

	if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
	{
		MLOG_ERROR("Could not create pipe!");
		return Tool();
	}

	t.out = g_hChildStd_IN_Wr;
	t.in = g_hChildStd_OUT_Rd;

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	BOOL success = CreateProcess(NULL, (LPSTR) name, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

	if (!success)
	{
		MLOG_ERROR("Could not start child!");
		return Tool();
	}

	if(input)
		t.write(input, strlen(input));

	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);

	CloseHandle(g_hChildStd_OUT_Wr);
	CloseHandle(g_hChildStd_IN_Rd);

	t.m_isrunning = true;
	return t;
}

size_t Tool::write(const char * buffer, size_t size)
{
	DWORD rbytes = 0;
	WriteFile(out, buffer, size, &rbytes, NULL);
	return rbytes; // (success) ? rbytes : 0;
}

size_t Tool::read(char * buffer, size_t size)
{
	DWORD rbytes = 0;
	ReadFile(in, buffer, size, &rbytes, NULL);
	return rbytes; // (success) ? rbytes : 0;
}

#endif

std::string Tool::executeToolBlocking(const char* name, const char* input)
{
	std::stringstream ss;
	Tool t = Tool::executeTool(name, input);

	if (!t.isRunning())
		return std::string();

#ifdef WIN32
	CloseHandle(t.out);
#endif

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


std::string Tool::executeToolNonBlocking(const char* name, const char* input)
{
	std::stringstream ss;
	Tool t = Tool::executeTool(name, input);

	if (!t.isRunning())
		return std::string();

#ifdef WIN32
	CloseHandle(t.out);
#endif

	std::atomic<bool> done(false);
	std::thread inthrd([&ss, &t, &done] () {
						   char* buffer = new char[256];
						   while (t.read(buffer, 255))
						   {
							   // Make sure the buffer is nul-terminated
							   buffer[255] = 0;
							   ss << buffer;
						   }

							delete[] buffer;
							done = true;
	});

	Neo::NeoGame* game = Neo::NeoEngine::getInstance()->getGame();
	while(!done)
	{
		game->update();
		game->draw();
	}

	inthrd.join();
	return ss.str();
}

