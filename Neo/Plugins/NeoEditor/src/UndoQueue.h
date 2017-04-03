#ifndef __UNDO_QUEUE_H
#define __UNDO_QUEUE_H

#include <string>

class UndoQueue
{
	int m_index;
	int m_undo;
	int m_maxIndex; /// Used to determine the number of files to delete for cleanup

	std::string m_tempdir;
	
	void load(int id);
	
public:
	UndoQueue();
	~UndoQueue();
	
	int getUndoIndex();
	int getRedoIndex();
	
	int doAction();
	void undo();
	void redo();

	void clear();	
	void save();
};

#endif
