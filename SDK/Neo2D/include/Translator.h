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

class NEO2D_EXPORT Translator
{
public:
	static const char* translate(const char* s);
	static void loadTranslation(const char* path);
};

}
}

#endif //NEO_TRANSLATOR_H
