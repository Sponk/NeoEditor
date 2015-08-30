
#ifndef ANDROID

#include <NeoEngine.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <imagehlp.h>
#endif

#if defined(__GNUC__) && !defined(_WIN32)
#include <execinfo.h>
#include <cxxabi.h>
#endif

/*
* Windows helper for POSIX functionality.
*/

#if defined(_MSC_VER)

#include <cstdio>
#include <cstdlib>

#define snprintf c99_snprintf

inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
	int count = -1;

	if (size != 0)
		count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
	if (count == -1)
		count = _vscprintf(format, ap);

	return count;
}

int c99_snprintf(char* str, size_t size, const char* format, ...)
{
	int count;
	va_list ap;

	va_start(ap, format);
	count = c99_vsnprintf(str, size, format, ap);
	va_end(ap);

	return count;
}
#endif

#if defined(_MSC_VER) || defined(_WIN32)
size_t c99_getline(char** lineptr, size_t* n, FILE* stream)
{
	char* bufptr = NULL;
	char* p = bufptr;
	size_t size;
	int c;

	if (lineptr == NULL)
	{
		return -1;
	}

	if (stream == NULL)
	{
		return -1;
	}

	if (n == NULL)
	{
		return -1;
	}

	bufptr = *lineptr;
	size = *n;

	if (feof(stream))
	{
		return -1;
	}

	c = fgetc(stream);

	if (bufptr == NULL)
	{
		bufptr = (char*)malloc(128);
		if (bufptr == NULL)
		{
			return -1;
		}
		size = 128;
	}

	p = bufptr;

	while (!feof(stream))
	{
		if ((p - bufptr) > (size - 1))
		{
			size = size + 128;
			bufptr = (char*)realloc(bufptr, size);

			if (bufptr == NULL)
			{
				return -1;
			}
		}

		*p++ = c;

		if (c == '\n')
		{
			break;
		}

		c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;
}


#endif // _MSC_VER


// Based on stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
// published under the WTFPL v2.0
std::string stacktrace(unsigned int max_frames = 63)
{
	std::string output = "Stacktraces are not available for your architecture!";

// Stacktrace for x86/64 GCC versions
#if !defined(__arm__) && !defined(_MSC_VER) && !defined(_WIN32)

	output = "Stacktrace:\n\n";

	void* addrlist[max_frames + 1];
	int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

	if (addrlen == 0)
	{
		output.append("  <empty, possibly corrupt>\n");
		return output;
	}

	char** symbollist = backtrace_symbols(addrlist, addrlen);
	std::string symbol;
	std::string function;
	std::string tmp;
	size_t size = 255;

	char demangled[255];

	int status;

	// Ignore first three stackframes since they only contain
	// Error handling
	for (int i = 3; i < addrlen; i++)
	{
		symbol = symbollist[i];
		symbol = symbol.substr(symbol.find_last_of("/") + 1);

		int idx = symbol.find("(") + 1;
		tmp = function = symbol.substr(idx, symbol.find_first_of('+') - idx);

		abi::__cxa_demangle(function.c_str(), demangled, &size, &status);
		function = demangled;

		switch (status)
		{
			case 0:
			{
				int idx = symbol.find(tmp);
				symbol.replace(idx, tmp.length(), "");
				symbol.insert(idx, function);
			}
			break;

			default:
				break;
		}

		output.append(symbol);
		output.push_back('\n');
	}
#endif
	return output;
}

#ifndef _WIN32
void crash_handler(int sig)
{
	std::string signal;
	switch (sig)
	{
		case SIGFPE:
			signal = "SIGFPE";
		case SIGILL:
			if (signal.empty())
				signal = "SIGFPE";
		case SIGSEGV:
			if (signal.empty())
				signal = "SIGSEGV";

			{
				char signum[3];
				snprintf(signum, 3, "%d", sig);
				std::string stack = stacktrace();
				std::string complete_text =
					"Cought signal " + string(signum) + " (" + signal + ")\n";

				complete_text += "\nSystem data:\n";

				FILE* pipe = popen("uname -p -o -r", "r");
				char c;
				while ((c = getc(pipe)) != EOF)
				{
					complete_text.push_back(c);
				}

				complete_text.append("\n");
				complete_text.append(stack);

				MLOG_ERROR(complete_text << endl);

				//std::string path = getenv("HOME");
				//path += "/.neoeditor/logfile.txt";

				//copyFile("logfile.txt", path.c_str());
				//system("./CrashHandler");

				exit(sig);
				break;
			}
	}
}
#endif

#ifdef _WIN32

std::string win32_stacktrace(CONTEXT* context)
{
	SymInitialize(GetCurrentProcess(), 0, true);
	std::string output;

	STACKFRAME frame = { 0 };

	frame.AddrPC.Offset = context->Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context->Esp;
	frame.AddrStack.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context->Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;

	char* symbolBuffer = new char[sizeof(IMAGEHLP_SYMBOL) + 255];
	memset(symbolBuffer, 0, sizeof(IMAGEHLP_SYMBOL) + 255);

	IMAGEHLP_SYMBOL* symbol = (IMAGEHLP_SYMBOL*)symbolBuffer;
	symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL) + 255;
	symbol->MaxNameLength = 254;

	unsigned int displacement = 0;

	char num[32];
	while (StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(),
					 GetCurrentThread(), &frame, context, 0,
					 SymFunctionTableAccess, SymGetModuleBase, 0))
	{
		sprintf(num, "0x%x", frame.AddrPC.Offset);

		if (SymGetSymFromAddr(GetCurrentProcess(), frame.AddrPC.Offset,
							  (DWORD*)&displacement, symbol))
		{
			output += symbol->Name;
			output += " [";
			output += num;
			output += "]\n";
		}
		else
		{
			output.append(string(num) + "\n");
		}
	}

	delete symbolBuffer;
	SymCleanup(GetCurrentProcess());
	return output;
}

LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS* ExceptionInfo)
{
	switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:
			MLOG_ERROR("EXCEPTION_ACCESS_VIOLATION");
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			MLOG_ERROR("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
			break;
		case EXCEPTION_BREAKPOINT:
			MLOG_ERROR("EXCEPTION_BREAKPOINT");
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			MLOG_ERROR("EXCEPTION_DATATYPE_MISALIGNMENT");
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			MLOG_ERROR("EXCEPTION_FLT_DENORMAL_OPERAND");
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			MLOG_ERROR("EXCEPTION_FLT_DIVIDE_BY_ZERO");
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			MLOG_ERROR("EXCEPTION_FLT_INEXACT_RESULT");
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			MLOG_ERROR("EXCEPTION_FLT_INVALID_OPERATION");
			break;
		case EXCEPTION_FLT_OVERFLOW:
			MLOG_ERROR("EXCEPTION_FLT_OVERFLOW");
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			MLOG_ERROR("EXCEPTION_FLT_STACK_CHECK");
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			MLOG_ERROR("EXCEPTION_FLT_UNDERFLOW");
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			MLOG_ERROR("EXCEPTION_ILLEGAL_INSTRUCTION");
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			MLOG_ERROR("EXCEPTION_IN_PAGE_ERROR");
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			MLOG_ERROR("EXCEPTION_INT_DIVIDE_BY_ZERO");
			break;
		case EXCEPTION_INT_OVERFLOW:
			MLOG_ERROR("EXCEPTION_INT_OVERFLOW");
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			MLOG_ERROR("EXCEPTION_INVALID_DISPOSITION");
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			MLOG_ERROR("EXCEPTION_NONCONTINUABLE_EXCEPTION");
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			MLOG_ERROR("EXCEPTION_PRIV_INSTRUCTION");
			break;
		case EXCEPTION_SINGLE_STEP:
			MLOG_ERROR("EXCEPTION_SINGLE_STEP");
			break;
		case EXCEPTION_STACK_OVERFLOW:
			MLOG_ERROR("EXCEPTION_STACK_OVERFLOW");
			break;
		default:
			MLOG_ERROR("Unrecognized Exception");
			break;
	}

	std::string stack;
	if (EXCEPTION_STACK_OVERFLOW !=
		ExceptionInfo->ExceptionRecord->ExceptionCode)
	{
		stack = win32_stacktrace(ExceptionInfo->ContextRecord);
	}

	char signum[3];
	snprintf(signum, 3, "%d", ExceptionInfo->ExceptionRecord->ExceptionCode);
	std::string complete_text = "Catched signal " + string(signum) + "\n";

	complete_text += "\nSystem data:\n";

	const char* env = getenv("OS");
	complete_text.append(env);
	complete_text.push_back(' ');

	env = getenv("PROCESSOR_ARCHITECTURE");
	complete_text.append(env);

	complete_text.append("\n\n");
	complete_text.append(stack);

	MLOG_ERROR(complete_text << endl);

	//std::string path = getenv("APPDATA");
	//path += "\\neoeditor\\logfile.txt";

	//copyFile("logfile.txt", path.c_str());
	//system(".\\CrashHandler.exe");

	exit(-1);
}
#endif

void registerDebugHandler()
{
// Set crash handler
#ifndef _WIN32
	signal(SIGFPE, crash_handler);
	signal(SIGILL, crash_handler);
	signal(SIGSEGV, crash_handler);
#else
	SetUnhandledExceptionFilter(windows_exception_handler);
#endif
}

#endif
