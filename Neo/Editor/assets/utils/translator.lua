--- translator.lua - This file contains basic primitives for handling and swapping
-- multiple translations for easy usage

local csv = dofile("csv.lua")

local translator = {}
translator.translations = {}
translator.currentTranslation = {}
translator.delimiter = ","

function translator.addTranslation(name, path)
   translator.translations[name] = csv.parseFile(path, translator.delimiter)
   translator.currentTranslation = translator.translations[name]
end

function translator.addTranslationString(name, src)
   translator.translations[name] = csv.parseString(src, translator.delimiter)
   translator.currentTranslation = translator.translations[name]
end

function translator.swapTranslation(name)
   translator.currentTranslation = translator.translations[name]
end

function tr(str)
   return translator.currentTranslation[str] or str
end

return translator
