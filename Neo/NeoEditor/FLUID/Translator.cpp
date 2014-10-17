#include "Translator.h"
#include <MEngine.h>

const char* tr(const char* key)
{
    return Translator::getInstance()->translate(key);
}

void Translator::loadTranslation(const char* file)
{
    if(!isFileExist(file))
		return;

    SAFE_DELETE(m_parser);
    m_parser = new INI::Parser(file);

#ifndef WIN32
    const char* substr = strrchr(file, '/');
#else
    const char* substr = strrchr(file, '\\');
#endif

    if(substr == NULL)
        substr = file;

    strcpy(m_langFile, ++substr);
}

const char* Translator::translate(const char* key)
{
    if(!m_parser)
        return key;

    std::string value = m_parser->top()("Keys")[key];
    if(value.empty())
        return key;

    return value.c_str();
}
