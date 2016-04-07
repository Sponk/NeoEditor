#include <Neo2DLevel.h>

Neo2D::Neo2DLevel* Neo2D::Neo2DLevel::getInstance()
{
	static Neo2D::Neo2DLevel m_instance;
	return &m_instance;
}

Neo::OText* Neo2D::Neo2DLevel::createText(const char* font, float size)
{
	if(!font)
		return NULL;

	Neo::SystemContext* system = Neo::NeoEngine::getInstance()->getSystemContext();

	char file[256];
	getGlobalFilename(file, system->getWorkingDirectory(), font);

	Neo::OText* text;
	Neo::FontRef* fontref = this->loadFont(file, size);

	text = new Neo::OText(fontref);
	text->setSize(fontref->getFont()->getFontSize());

	return text;
}