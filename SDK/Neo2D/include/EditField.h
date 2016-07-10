#ifndef NEO_EDITFIELD_H
#define NEO_EDITFIELD_H

#include <Widget.h>
#include "CommonEvents.h"

#include <string>

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a one line input field with UTF-8 support.
 */
class NEO2D_EXPORT EditField : public Widget
{
	size_t m_caret;
public:
	EditField(int x,
			  int y,
			  unsigned int w,
			  unsigned int h,
			  const char* label,
			  const shared_ptr<Object2D>& parent,
			  const shared_ptr<Theme>& theme = nullptr);

	/**
	 * @brief Sets the position of the text cursor.
	 * @param i The new position in bytes (NOT CHARACTERS!)
	 */
	void setCaret(size_t i) { m_caret = i; }

	/**
	 * @brief Retrieves the position of the text cursor.
	 * @return The position in bytes (NOT CHARACTERS!)
	 */
	size_t getCaret() const { return m_caret; }

	static unsigned int characterSize(unsigned int c);

	virtual bool handle(const Event& e) override;
	virtual void init() override;
};

}
}
#endif //NEO_EDITFIELD_H
