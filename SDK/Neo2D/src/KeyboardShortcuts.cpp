#include <KeyboardShortcuts.h>
#include <CommonEvents.h>

Neo2D::Gui::KeyboardShortcuts::KeyboardShortcuts(const shared_ptr<Widget>& parent)
	: Widget(0, 0, 0, 0, nullptr, parent, nullptr)
{}

void Neo2D::Gui::KeyboardShortcuts::init()
{
	registerEvent(make_shared<KeyPressEvent>(shared_from_this(), nullptr, nullptr));
	registerEvent(make_shared<KeyReleaseEvent>(shared_from_this(), nullptr, nullptr));
}

bool Neo2D::Gui::KeyboardShortcuts::handle(const Neo2D::Gui::Event& e)
{
	switch(e.getType())
	{
		case Neo2D::Gui::KEY_PRESSED:
		{
			bool triggered = false;
			auto key = static_cast<const Neo2D::Gui::KeyPressEvent&>(e).getKey();
			for(int i = 0; i < shortcuts.size(); i++)
			{
				if(shortcuts[i].update(key, true)) return true;
			}
			return false;
		}

		case Neo2D::Gui::KEY_RELEASED:
		{
			auto key = static_cast<const Neo2D::Gui::KeyPressEvent&>(e).getKey();
			for(int i = 0; i < shortcuts.size(); i++)
			{
				shortcuts[i].update(key, false);
			}
			return false;
		}
	}
	return false;
}

bool Neo2D::Gui::Shortcut::update(unsigned int key, bool value)
{
	status[key] = value;
	bool triggered = true;
	for(auto k : keys)
		triggered &= status[k];

	if(triggered && callback)
		callback(data);

	return triggered;
}
