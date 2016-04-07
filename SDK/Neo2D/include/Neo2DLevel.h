#ifndef NEO2D_LEVEL_H
#define NEO2D_LEVEL_H

#include <Neo2D.h>
#include <NeoEngine.h>

namespace Neo2D
{
	/**
	 * @brief A 2D level which provides data management capabilities.
	 * 
	 * Uses a Neo::Level but only exposes features needed for 2D operation.
	 */
	class NEO2D_EXPORT Neo2DLevel
	{
		Neo::Level m_level;
	public:
		Neo::FontRef* loadFont(const char* filename, unsigned int fontsize)
		{
			return m_level.loadFont(filename, fontsize);
		}

		Neo::TextureRef* loadTexture(const char* filename, const bool mipmap = true, const bool preload = true)
		{
			return loadTexture(filename, mipmap, preload);
		}

		void clear() { m_level.clear(); }

		Neo::OText* Neo2D::Neo2DLevel::createText(const char* font, float size);
		static Neo2DLevel* getInstance();
	};
}

#endif