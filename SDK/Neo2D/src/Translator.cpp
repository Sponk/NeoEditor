#include <Translator.h>

#define CSV_IO_NO_THREAD
#include <csv.h>

using namespace Neo2D;
using namespace Gui;

static std::unordered_map<std::string, std::string> m_strings;
const char* Translator::translate(const char* s)
{
	auto iter = m_strings.find(s);
	if(iter == m_strings.end())
		return s;

	return iter->second.c_str();
}

void Translator::loadTranslation(const char* path)
{
	io::CSVReader<2, io::trim_chars<>, io::no_quote_escape<'\t'>> in(path);
	//in.read_header(io::ignore_missing_column, "key", "translation");

	std::string key, value;
	while(in.read_row(key, value))
	{
		m_strings[key] = value;
		value.clear();
	}
}