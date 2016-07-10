#ifndef NEO_TRANSLATOR_H
#define NEO_TRANSLATOR_H

#include <Neo2D.h>
#include <unordered_map>
#include <string>

#define tr(s) Neo2D::Gui::Translator::translate(s)

namespace Neo2D
{
namespace Gui
{

/**
 * @brief Implements a few static methods that can be used to translate strings.
 *
 * It reads in CSV files which contain key value pairs and translates between them while
 * preserving strings which do not appear in the translation files.
 * Easy access is provided by the "tr" macro which accesses the translate method so translating a string
 * becomes an easy call to 'tr("somestring")' rather than the long way.
 */
class NEO2D_EXPORT Translator
{
	static std::unordered_map<std::string, std::string> m_strings;

public:
	/**
	 * @brief Translates a given string into another one.
	 * @param s The string to translate.
	 * @return The translated string or the input value if no translation exists.
	 */
	static const char* translate(const char* s);

	/**
	 * @brief Loads a CSV formatted translation file.
	 *
	 * The file has to contain rows with two string values each.
	 * The values have to be seperated by the tab character ('\t').
	 *
	 * @param path The file to load.
	 * @return Success.
	 */
	static bool loadTranslation(const char* path);

	static void clear()
	{
		m_strings.clear();
	}
};

}
}

#endif //NEO_TRANSLATOR_H
