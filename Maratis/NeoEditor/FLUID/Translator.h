#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "ini.h"

const char* tr(const char* key);

class Translator
{
    INI::Parser* m_parser;
    char m_langFile[255];

public:
    static Translator* getInstance() { static Translator m_tr; return &m_tr; }

    Translator(): m_parser(NULL) {}
    void loadTranslation(const char* file);
    const char* translate(const char* key);
    const char* getLanguageFile() { return m_langFile; }
};

#endif
