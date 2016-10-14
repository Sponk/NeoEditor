#include <Translator.h>
#include <NeoEngine.h>
#define CSV_IO_NO_THREAD
#include <csv.h>

using namespace Neo2D;
using namespace Gui;

NEO2D_EXPORT std::unordered_map<std::string, std::string> Translator::m_strings;

const char* Translator::translate(const char* s)
{
	auto iter = m_strings.find(s);
	if(iter == m_strings.end())
		return s;

	return iter->second.c_str();
}

bool Translator::loadTranslation(const char* path)
{
	try
	{
		io::CSVReader<2, io::trim_chars<>, io::no_quote_escape<'\t'>> in(path);

		std::string key, value;
		while(in.read_row(key, value))
		{
			m_strings[key] = value;
			value.clear();
		}
	}
	catch(std::exception& e)
	{
		return false;
	}

	return true;
}
