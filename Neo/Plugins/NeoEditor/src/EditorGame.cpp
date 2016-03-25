#include "EditorGame.h"
#include "Tool.h"

void EditorGame::update()
{

}

void EditorGame::draw()
{

}

void EditorGame::onBegin()
{
	std::string str = Tool::executeToolBlocking("cmake.exe", NULL);
	MLOG_INFO(str);
}

void EditorGame::onEnd()
{

}