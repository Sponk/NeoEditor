#ifndef __UNDO_QUEUE_H
#define __UNDO_QUEUE_H

#include <string>

class UndoQueue
{
	int m_index;
	int m_undo;

	std::string m_tempdir;
	
	void load(int id);
	
public:
	UndoQueue();
	
	int getUndoIndex();
	int getRedoIndex();
	
	int doAction();
	void undo();
	void redo();

	void clear() { m_index = 0; m_undo = 0; }
	
	void save();
};

#endif
