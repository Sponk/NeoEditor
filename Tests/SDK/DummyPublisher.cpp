
#include <NeoEngine.h>
#include <../../../Neo/NeoEditor/Publish/Publisher.h>
#include <LuaScript.h>

namespace Neo
{
	Publisher::Publisher() {}
	Publisher::~Publisher() {}
	void Publisher::publish(const char* proj) {}
}

void beginDraw(Neo::PRIMITIVE_TYPES) {}
void pushVertex(const Neo::Vector3& v) {}
void endDraw(Neo::RenderingContext* c) {}

void createFltkLuaBindings(Neo::LuaScript*) {}
