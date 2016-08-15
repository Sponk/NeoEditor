#include <Translator.h>

#define CSV_IO_NO_THREAD
#include <csv.h>

using namespace Neo2D;
using namespace Gui;

std::unordered_map<std::string, std::string>& Translator::getStrings()
{
	static std::unordered_map<std::string, std::string> m_strings;
	return m_strings;
}

const char* Translator::translate(const char* s)
{
	auto m_strings = getStrings();
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
		auto m_strings = getStrings();

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