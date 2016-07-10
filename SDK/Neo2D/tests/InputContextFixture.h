#ifndef __INPUT_CONTEXT_FIXTURE_H
#define __INPUT_CONTEXT_FIXTURE_H

#include <gtest/gtest.h>
#include <NeoEngine.h>

class InputContextFixture : public ::testing::Test
{
public:
	Neo::InputContextDummy m_input;
	Neo::Mouse& m_mouse = m_input.getMouse();
	Neo::Keyboard& m_kbd = m_input.getKeyboard();
protected:
	virtual void SetUp()
	{
		Neo::NeoEngine::getInstance()->setInputContext(&m_input);
	}

	virtual void TearDown()
	{
		m_input.flush();
		m_mouse.setDirection(Neo::Vector2(0,0));
		m_mouse.moveCursor(Neo::Vector2(0,0));

		Neo::NeoEngine::getInstance()->setInputContext(nullptr);
	}
};

#endif
