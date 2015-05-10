#include "MonoSubgame.h"
#include "MonoContext.h"

using namespace Neo;

MonoSubgame::MonoSubgame()
{

}

MonoSubgame::~MonoSubgame()
{

}

void MonoSubgame::onBegin()
{
	MLOG_INFO("Initializing MonoSubgame");
	MonoContext::getInstance()->callMethod("Game:onBegin()");
}

void MonoSubgame::update()
{
	MonoContext::getInstance()->callMethod("Game:update()");
}

void MonoSubgame::onEnd()
{
	MLOG_INFO("Cleaning up MonoSubgame");
	MonoContext::getInstance()->callMethod("Game:onEnd()");
}
