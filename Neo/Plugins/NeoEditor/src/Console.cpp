#include "Console.h"
#include "Sidepanel.h"
#include <VerticalLayout.h>

using namespace Neo;
using namespace Neo2D;
using namespace Gui;


class ConsoleScriptLogger : public ScriptLogger
{
	Console* console;
public:
	ConsoleScriptLogger(Console* console) :
		console(console)
	{ }
	
	virtual void print(const char* str)
	{
		MLOG_INFO(str);
		console->write(str);
	}
};

Console::Console(int x,
				 int y,
				 unsigned int w,
				 unsigned int h,
				 const shared_ptr<Object2D>& parent)
	: Widget(x, y, w, h, nullptr, parent, nullptr)
{
	NeoEngine::getInstance()->getScriptContext()->setScriptLogger(make_shared<ConsoleScriptLogger>(this));
}

void Console::init()
{
	scrollPanel = make_shared<ScrollPanel>(0,0,0,0, shared_from_this());
	commandEdit = make_shared<EditField>(0,0,0,0,"", shared_from_this());
	
	commandOutput = make_shared<Label>(0,0,0,0,"", scrollPanel);
	commandOutput->setColor(Vector4(0,0,0,1));

	auto layout = make_shared<VerticalLayout>();
	scrollPanel->setLayout(layout);
	layout->enableResize(false);
	
	scrollPanel->addWidget(commandOutput);	
	scrollPanel->init();

	commandEdit->setCallback([this](Widget& w, void*){
		NeoEngine::getInstance()->getScriptContext()->runString(commandEdit->getLabel());

		commandEdit->setCaret(0);
		commandEdit->setLabel("");
	}, nullptr);
}

bool Console::handle(const Neo2D::Gui::Event& e)
{
	return false;
}

void Console::update(float dt)
{
	Widget::update(dt);

	scrollPanel->setPosition(getPosition());
	scrollPanel->setSize(Vector2(getSize().x - 30, getSize().y - 20));

	commandEdit->setPosition(getPosition() + Vector2(-2, scrollPanel->getSize().y));
	commandEdit->setSize(Vector2(scrollPanel->getSize().x + 15, 25));
	
	scrollPanel->update(dt);
	commandEdit->update(dt);
}

void Console::draw(const Neo::Vector2& offset)
{
	scrollPanel->draw(offset);
	commandEdit->draw(offset);
}

void Console::write(const char* str)
{
	std::string out = commandOutput->getLabel();
	out += str;
	commandOutput->setLabel(out.c_str());

	size_t lines = std::count(out.begin(), out.end(), '\n');

	// FIXME: Use actual font size!
	commandOutput->setSize(Vector2(getSize().x, static_cast<float>(lines * 12)));
}
