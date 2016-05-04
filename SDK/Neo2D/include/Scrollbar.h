#ifndef NEO_SCROLLBAR_H
#define NEO_SCROLLBAR_H

#include <Widget.h>

namespace Neo2D
{
namespace Gui
{

enum SCROLLBAR_DIRECTION
{
	SCROLLBAR_VERTICAL,
	SCROLLBAR_HORIZONTAL
};

class NEO2D_EXPORT Scrollbar : public Widget, public std::enable_shared_from_this<Scrollbar>
{
	float value;
	Neo::Vector2 range;
	SCROLLBAR_DIRECTION direction;

protected:
	class Knob : public Widget
	{
		Scrollbar& scrollbar;
	public:
		Knob(int x,
			 int y,
			 unsigned int w,
			 unsigned int h,
			 Scrollbar& s,
			 const shared_ptr<Theme>& theme)
			: Widget(x, y, w, h, nullptr, nullptr, theme), scrollbar(s)
		{ }

		virtual void handle(const Event& e) override
		{
			scrollbar.handle(e);
		}
	} knob;

	Knob& getKnob() { return knob; }
public:
	Scrollbar(int x,
			  int y,
			  unsigned int w,
			  unsigned int h,
			  const shared_ptr<Object2D>& parent,
			  const shared_ptr<Theme>& knobtheme = nullptr,
			  const shared_ptr<Theme>& background = nullptr);

	float getValue() const
	{
		return value;
	}

	void setValue(float val)
	{
		float length = (direction == SCROLLBAR_HORIZONTAL) ? getSize().x : getSize().y;
		value = MAX(range.x, MIN(val, (range.y - range.x) - length));
	}

	const Neo::Vector2& getRange() const
	{
		return range;
	}

	float calculateKnobsize() const
	{
		if(getDirection() == SCROLLBAR_VERTICAL)
			return MIN(getSize().y, MAX(10.0, getSize().y * (getSize().y / (getRange().y - getRange().x))));
		else
			return MIN(getSize().x, MAX(10.0, getSize().x * (getSize().x / (getRange().y - getRange().x))));
	}

	void setRange(const Neo::Vector2& range)
	{
		Scrollbar::range = range;

		if(getDirection() == SCROLLBAR_HORIZONTAL)
		{
			knob.setSize(Neo::Vector2(calculateKnobsize(), getSize().y));
		}
		else
		{
			knob.setSize(Neo::Vector2(getSize().x, calculateKnobsize()));
		}
	}

	SCROLLBAR_DIRECTION getDirection() const
	{
		return direction;
	}

	void setDirection(SCROLLBAR_DIRECTION direction)
	{
		Scrollbar::direction = direction;

		// Update range
		setRange(getRange());
	}
	
	virtual void handle(const Neo2D::Gui::Event & e) override;
	virtual void draw(const Neo::Vector2& offset) override
	{
		Widget::draw(offset);
		knob.draw(offset);
	}
};

}
}

#endif //NEO_SCROLLBAR_H
