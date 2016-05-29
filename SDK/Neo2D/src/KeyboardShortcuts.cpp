#include <KeyboardShortcuts.h>
#include <CommonEvents.h>

Neo2D::Gui::KeyboardShortcuts::KeyboardShortcuts()
	: Widget(0, 0, 0, 0, nullptr, nullptr, nullptr)
{}

bool Neo2D::Gui::KeyboardShortcuts::handle(const Neo2D::Gui::Event& e)
{
	switch(e.getType())
	{
		case Neo2D::Gui::KEY_PRESSED:
		{
			auto key = static_cast<const Neo2D::Gui::KeyPressEvent&>(e).getKey();
			for(int i = 0; i < shortcuts.size(); i++)
			{
				shortcuts[i].update(key, true);
			}
			return true;
		}

		case Neo2D::Gui::KEY_RELEASED:
		{
			auto key = static_cast<const Neo2D::Gui::KeyPressEvent&>(e).getKey();
			for(int i = 0; i < shortcuts.size(); i++)
			{
				shortcuts[i].update(key, false);
			}
			return true;
		}
	}

	return false;
}

void Neo2D::Gui::Shortcut::update(unsigned int key, bool value)
{
	status[key] = value;
	bool triggered = true;
	for(auto k : keys)
		triggered &= status[k];

	if(triggered && callback)
		callback(data);
}