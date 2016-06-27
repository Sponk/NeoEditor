//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#ifndef __SCRIPT_CONTEXT_H
#define __SCRIPT_CONTEXT_H

#include <memory>

namespace Neo
{

class NEO_CORE_EXPORT ScriptLogger
{
 public:
	virtual void print(const char* str)
	{
		printf("%s", str);
	}
};
	
/// Class used to manage script functions virtually.
class NEO_CORE_EXPORT ScriptContext
{
protected:
	bool m_isRunning;
	shared_ptr<ScriptLogger> m_logger;
	
public :

	ScriptContext() : m_logger(make_shared<ScriptLogger>()) {}
	
	/// Destructor.
	virtual ~ScriptContext(void){}

	void setScriptLogger(const shared_ptr<ScriptLogger>& log) { m_logger = log; }
	const shared_ptr<ScriptLogger>& getScriptLogger() const { return m_logger; }
	
	virtual void init() = 0;

	/// Run script.
	/// \param filename	Script filename
    virtual bool runScript(const char * filename) = 0;

    /// Start call function. All arguments to the function have to be pushed
    /// between ScriptContext::startCallFunction and ScriptContext::endCallFunction
	/// \param name		Function name
	virtual bool startCallFunction(const char * name) = 0;
	
	/// End call script function.
	/// \param numArgs	Number of arguments
	virtual bool endCallFunction(int numArgs = 0) = 0;
	
	/// Call script function.
	/// \param name		Function name
	virtual void callFunction(const char * name) = 0;

	/// Add script function.
	/// \param name		Function name
	/// \param function	Function callback
	virtual void addFunction(const char * name, int (*function)(void)) = 0;

	virtual bool runString(const char* str) = 0;

	// variables
	virtual unsigned int getArgsNumber(void) = 0;

	virtual void getIntArray(unsigned int arg, int * values, unsigned int valuesNumber) = 0;
	virtual void getFloatArray(unsigned int arg, float * values, unsigned int valuesNumber) = 0;
	virtual const char * getString(unsigned int arg) = 0;
	virtual int getInteger(unsigned int arg) = 0;
	virtual float getFloat(unsigned int arg) = 0;
	virtual void * getPointer(unsigned int arg) = 0;
	virtual bool getBoolean(unsigned int arg) = 0;

    virtual void pushIntArray(const int * values, unsigned int valuesNumber) = 0;
	virtual void pushFloatArray(const float * values, unsigned int valuesNumber) = 0;
	virtual void pushString(const char * string) = 0;
	virtual void pushBoolean(bool value) = 0;
    virtual void pushInteger(int value) = 0;
	virtual void pushFloat(float value) = 0;
	virtual void pushPointer(void * value) = 0;

	// Type checking
	virtual bool isNumber(unsigned int arg) = 0;
	virtual bool isFunctionOk(const char* name, unsigned int args) = 0;

	bool isRunning() { return m_isRunning; }

	/**
	 * @brief If the script is marked as running, mark it as not running.
	 */
	bool stopRunning() { if(m_isRunning) m_isRunning = false; }
};

class NEO_CORE_EXPORT ScriptContextDummy : public ScriptContext
{
public:
	virtual ~ScriptContextDummy(void){}
	virtual void init() {}

	virtual bool runScript(const char * filename) { return false; }
	virtual bool startCallFunction(const char * name) { return false; }
	virtual bool endCallFunction(int numArgs = 0) { return false; }
	virtual void callFunction(const char * name) { }
	virtual void addFunction(const char * name, int (*function)(void)) {}
	virtual bool runString(const char* str) { return true; }
	virtual unsigned int getArgsNumber(void) { return 0; }
	virtual void getIntArray(unsigned int arg, int * values, unsigned int valuesNumber) {}
	virtual void getFloatArray(unsigned int arg, float * values, unsigned int valuesNumber) {}
	virtual const char * getString(unsigned int arg) { return ""; }
	virtual int getInteger(unsigned int arg) { return 0; }
	virtual float getFloat(unsigned int arg) { return 0.0f; }
	virtual void * getPointer(unsigned int arg) { return NULL; }
	virtual bool getBoolean(unsigned int arg) { return false; }

	virtual void pushIntArray(const int * values, unsigned int valuesNumber) {}
	virtual void pushFloatArray(const float * values, unsigned int valuesNumber) {}
	virtual void pushString(const char * string) {}
	virtual void pushBoolean(bool value) {}
	virtual void pushInteger(int value) {}
	virtual void pushFloat(float value) {}
	virtual void pushPointer(void * value) {}

	// Type checking
	virtual bool isNumber(unsigned int arg) { return false; }
	virtual bool isFunctionOk(const char* name, unsigned int args) { return true; }
};
}
#endif
