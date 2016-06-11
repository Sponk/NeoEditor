#ifndef NEOPLUGIN_MONOCONTEXT_H_
#define NEOPLUGIN_MONOCONTEXT_H_

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

class MonoContext
{
	MonoDomain* domain;
	MonoAssembly* assembly;
	bool m_debug;

	void clear();

public:
	MonoContext();
	virtual ~MonoContext();

	bool loadAssembly(const char* path);
	bool addAssembly(const char* path);

	void callMethod(const char* name);
	void attachToThread();

	void enableDebugging(bool value) { m_debug = value; }

	static MonoContext* getInstance() { static MonoContext context; return &context; }
};

#endif /* NEOPLUGIN_MONOCONTEXT_H_ */
