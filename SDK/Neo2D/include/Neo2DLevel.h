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

		/**
		 * @brief Loads a font into memory.
		 * @see Neo::Level::loadFont
		 */
		Neo::FontRef* loadFont(const char* filename, unsigned int fontsize)
		{
			return m_level.loadFont(filename, fontsize);
		}

		/**
		 * @brief Loads a texture into memory.
		 * @see Neo::Level::loadTexture
		 */
		Neo::TextureRef* loadTexture(const char* filename, const bool mipmap = true, const bool preload = true)
		{
			return m_level.loadTexture(filename, mipmap, preload);
		}

		/**
		 * @brief Clears the level.
		 * @see Neo::Level::clear
		 */
		void clear() { m_level.clear(); }

		/**
		 * @brief Creates a new text object for 2D rendering.
		 *
		 * @param font The font file to use.
		 * @param size The font size.
		 * @return The newly created text object.
		 */
		Neo::OText* createText(const char* font, float size);

		/**
		 * @brief Retrieves the Neo2DLevel singleton object.
		 * @return The singleton.
		 */
		static Neo2DLevel* getInstance();
	};
}

#endif