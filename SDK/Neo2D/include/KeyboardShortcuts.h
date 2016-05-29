#ifndef NEO_KEYBOARDSHORTCUTS_H
#define NEO_KEYBOARDSHORTCUTS_H

#include <Widget.h>
#include <vector>
#include <map>

namespace Neo2D
{
namespace Gui
{

class NEO2D_EXPORT Shortcut
{
private:
	std::map<unsigned int, bool> status;
	std::vector<Neo::INPUT_KEYS> keys;
	std::function<void(void*)> callback;
	void* data;

public:
	Shortcut(const std::vector<Neo::INPUT_KEYS>& keys, std::function<void(void*)> callback, void* data) :
		callback(callback),
		data(data),
		keys(keys)
	{}

	bool update(unsigned int key, bool value);
};

class NEO2D_EXPORT KeyboardShortcuts : public Widget
{
	std::vector<Shortcut> shortcuts;
public:
	KeyboardShortcuts(const shared_ptr<Widget>& parent = nullptr);
	virtual bool handle(const Event& e) override;
	virtual void init() override;

	void addShortcut(const Shortcut& s)
	{
		shortcuts.push_back(s);
	}
};

}
}
#endif //NEO_KEYBOARDSHORTCUTS_H
