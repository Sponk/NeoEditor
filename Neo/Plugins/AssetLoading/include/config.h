#ifndef CONFIG_H
#define CONFIG_H

#ifdef _WIN32
	#define EXPORT __declspec(dllexport)
#else
	#define EXPORT
#endif

#endif // CONFIG_H
