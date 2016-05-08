#ifndef NEO_SCROLLBAR_H
#define NEO_SCROLLBAR_H

#include <Widget.h>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Possible directions for a scroll bar.
 */
enum SCROLLBAR_DIRECTION
{
	SCROLLBAR_VERTICAL,
	SCROLLBAR_HORIZONTAL
};

/**
 * @brief Implements a basic scroll bar.
 */
class NEO2D_EXPORT Scrollbar : public Widget, public std::enable_shared_from_this<Scrollbar>
{
	float value;
	Neo::Vector2 range;
	SCROLLBAR_DIRECTION direction;

protected:

	/**
	 * @brief A grabbable knob that is used to alter the value of a scroll bar.
	 */
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
			  const SCROLLBAR_DIRECTION direction = SCROLLBAR_HORIZONTAL,
			  const shared_ptr<Theme>& knobtheme = nullptr,
			  const shared_ptr<Theme>& background = nullptr);

	/**
	 * @brief Retrieves the current value.
	 * @return The current value.
	 */
	float getValue() const
	{
		return value;
	}

	/**
	 * @brief Changes the current value.
	 *
	 * The value will be altered to be a valid value for the current scroll bar configuration.
	 *
	 * @param val The new value.
	 */
	void setValue(float val)
	{
		float length = (direction == SCROLLBAR_HORIZONTAL) ? getSize().x : getSize().y;
		value = MAX(range.x, MIN(val, (range.y - range.x) - length));
	}

	/**
	 * @brief Retrieves the current range.
	 *
	 * The range is a 2D vector containing the start value and end value of the scrollbar in pixels.
	 * It uses pixels to directly interact with the size of a container and the ratio of the range and
	 * the width/height of the scrollbar is used to determine the knob size.
	 *
	 * @return The current range.
	 */
	const Neo::Vector2& getRange() const
	{
		return range;
	}

	/**
	 * @brief Calculates the size of the knob for the current configuration.
	 * @return The size of the knob.
	 */
	float calculateKnobsize() const
	{
		if(getDirection() == SCROLLBAR_VERTICAL)
			return MIN(getSize().y, MAX(10.0, getSize().y * (getSize().y / (getRange().y - getRange().x))));
		else
			return MIN(getSize().x, MAX(10.0, getSize().x * (getSize().x / (getRange().y - getRange().x))));
	}

	/**
	 * @brief Changes the current range.
	 *
	 * Also updates the size of the knob.
	 * @param range The new range.
	 *
	 * @see getRange
	 */
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

	/**
	 * @brief Update the position of the knob by using the position of the scrollbar.
	 *
	 * Resets the value to 0.
	 */
	void updateKnobPosition()
	{
		knob.setPosition(getPosition());
		setValue(0);
	}

	/**
	 * @brief Retrieves the scroll bar direction.
	 * @return The direction of the scroll bar.
	 */
	SCROLLBAR_DIRECTION getDirection() const
	{
		return direction;
	}

	/**
	 * @brief Changes the direction of the scroll bar.
	 *
	 * This also updates the range i.e. the size of the knob.
	 * @param direction The new direction.
	 */
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
