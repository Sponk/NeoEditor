#include <Sprite.h>
#include <Neo2DLevel.h>

using namespace Neo2D;

Sprite::Sprite(int x, int y, unsigned int w, unsigned int h, const char* image, const shared_ptr<Object2D>& parent)
	: Object2D(x, y, w, h, parent), m_imagePath(image), m_image(0), m_flip(0,180)
{
	setRotation(180);
	loadTexture();
}

// Can't test rendering
// LCOV_EXCL_START
void Sprite::draw(const Neo::Vector2& offset)
{
	Neo::Renderer* render = Neo::NeoEngine::getInstance()->getRenderer();
	render->drawTexturedQuad(getPosition() + offset, getSize(), m_image, getRotation(), Neo::Vector2(1,1), getFlip());
}
// LCOV_EXCL_STOP

void Sprite::loadTexture()
{
	Neo2DLevel* level = Neo2DLevel::getInstance();
	Neo::SystemContext* system = Neo::NeoEngine::getInstance()->getSystemContext();

	char buf[256];
	getGlobalFilename(buf, system->getWorkingDirectory(), m_imagePath.getSafeString());

	Neo::TextureRef* tex = level->loadTexture(buf);
	tex->update();

	Neo::Vector2 imageSize = Neo::Vector2(tex->getWidth(), tex->getHeight());
	m_image = tex->getTextureId();

	if (getSize() == Neo::Vector2(0,0))
		setSize(imageSize);
}
