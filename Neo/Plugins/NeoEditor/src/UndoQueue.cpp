#include "UndoQueue.h"
#include <NeoEngine.h>
#include <sstream>
#include <cstdlib>

#undef max
#undef min
#include <algorithm>

using namespace Neo;

UndoQueue::UndoQueue()
	: m_index(0),
	  m_undo(0)
{
	#ifndef WIN32
	m_tempdir = "/tmp";
	#else
	m_tempdir = getenv("TEMP");
	#endif
}

int UndoQueue::getUndoIndex()
{
	return m_undo = std::max(1, m_undo - 1);
}

int UndoQueue::getRedoIndex()
{
	return m_undo = std::min(m_index, m_undo + 1);	
}

void UndoQueue::undo()
{
	load(getUndoIndex());
}

void UndoQueue::redo()
{
	load(getRedoIndex());
}

void UndoQueue::load(int id)
{
	if(id <= 0 || id > m_index)
		return;

	NeoEngine* engine = NeoEngine::getInstance();
	std::stringstream ss;
	ss << m_tempdir << "/" << "neo_autosave_" << id << ".llvl";
	
	// MLOG_INFO("Loading quicksave from " << ss.str());
	
	engine->getLevelLoader()->loadData(ss.str().c_str(), engine->getLevel());
	engine->getLevel()->getCurrentScene()->updateObjectsMatrices();
}

void UndoQueue::save()
{
	NeoEngine* engine = NeoEngine::getInstance();
	std::stringstream ss;
	ss << m_tempdir << "/" << "neo_autosave_" << doAction() << ".llvl";

	// MLOG_INFO("Saving quicksave to " << ss.str());
	engine->getLevelLoader()->saveData(ss.str().c_str(), "llvl", engine->getLevel());
}

int UndoQueue::doAction()
{
	return (m_index = ++m_undo);
}
