#include <Label.h>
#include <NeoEngine.h>
#include <Neo2DLevel.h>

using namespace Neo;

Neo2D::Gui::Label::Label(int x, int y, unsigned int w, unsigned int h, const char* label, const shared_ptr<Object2D>& parent) :
	Widget(x, y, w, h, label, parent, nullptr),
	m_color(1,1,1,1),
	m_alignment(Neo::TEXT_ALIGN_LEFT),
	m_font("assets/default.ttf"),
	m_text(nullptr),
	m_fontsize(12)
{}

static void draw2DText(OText* text, float x, float y, float rotation)
{
	RenderingContext* renderContext =
		NeoEngine::getInstance()->getRenderingContext();

	text->setPosition(Vector3(floor(x), floor(y), 0));
	text->setRotation(Quaternion(0, 0, rotation));
	text->updateMatrix();

	renderContext->pushMatrix();
	renderContext->multMatrix(text->getMatrix());

	NeoEngine::getInstance()->getRenderer()->drawText(text);
	renderContext->popMatrix();
}

void Neo2D::Gui::Label::draw(const Neo::Vector2& offset)
{
	const Neo::Vector2 position = getPosition() + offset;

	if(!m_text)
		initText();

	m_text->setAlign(getAlignment());
	m_text->setColor(getColor());
	m_text->setText(getLabel());

	draw2DText(m_text, position.x,
			   position.y + m_text->getSize() * 0.7, 0.0f);
}

void Neo2D::Gui::Label::initText()
{
	m_text = Neo2D::Neo2DLevel::getInstance()->createText(getFont(), getFontsize());
}

void Neo2D::Gui::Label::setFontsize(unsigned int fontsize)
{
	if(m_fontsize == fontsize)
		return;

	SAFE_DELETE(m_text);
	m_fontsize = fontsize;

	initText();
}

void Neo2D::Gui::Label::setFont(const char* font)
{
	if(m_font == font)
		return;

	SAFE_DELETE(m_text);
	m_font = font;

	initText();
}
