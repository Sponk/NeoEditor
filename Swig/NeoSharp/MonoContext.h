#ifndef NEOPLUGIN_MONOCONTEXT_H_
#define NEOPLUGIN_MONOCONTEXT_H_

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

class MonoContext
{
	MonoDomain* domain;
	MonoAssembly* assembly;

	void clear();

public:
	MonoContext();
	virtual ~MonoContext();

	bool loadAssembly(const char* path);
	bool addAssembly(const char* path);

	void callMethod(const char* name);
	void attachToThread();

	static MonoContext* getInstance() { static MonoContext context; return &context; }
};

#endif /* NEOPLUGIN_MONOCONTEXT_H_ */
