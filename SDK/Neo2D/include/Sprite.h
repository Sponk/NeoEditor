#ifndef NEO_SPRITE_H
#define NEO_SPRITE_H

#include <Neo2D.h>
#include <Object2D.h>

namespace Neo2D
{

class NEO2D_EXPORT Sprite: public Object2D
{
	Neo::String m_imagePath;
	unsigned int m_image;
	Neo::Vector2 m_flip;

protected:
	void loadTexture();

public:
	Sprite(int x, int y, unsigned int w, unsigned int h, const char* image, const shared_ptr<Object2D>& parent);
	virtual void draw(const Neo::Vector2& offset);

	const Neo::Vector2& getFlip() const
	{
		return m_flip;
	}

	void setFlip(const Neo::Vector2& m_flip)
	{
		Sprite::m_flip = m_flip;
	}
};

}

#endif //NEO_SPRITE_H
