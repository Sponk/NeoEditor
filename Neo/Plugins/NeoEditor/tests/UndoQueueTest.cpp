#include <gtest/gtest.h>
#include "../src/UndoQueue.h"

TEST(UndoQueueTest, UndoIndexTest)
{
	UndoQueue q;
	q.doAction();
	q.doAction();
	q.doAction();

	EXPECT_EQ(2, q.getUndoIndex());
	EXPECT_EQ(1, q.getUndoIndex());
	EXPECT_EQ(1, q.getUndoIndex());
}

TEST(UndoQueueTest, RedoIndexTest)
{
	UndoQueue q;
	q.doAction();
	q.doAction();
	q.doAction();

	EXPECT_EQ(2, q.getUndoIndex());
	EXPECT_EQ(1, q.getUndoIndex());
	EXPECT_EQ(2, q.getRedoIndex());
	EXPECT_EQ(3, q.getRedoIndex());
	EXPECT_EQ(3, q.getRedoIndex());	
}
