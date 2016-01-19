--- translator.lua - This file contains basic primitives for handling and swapping
-- multiple translations for easy usage

local csv = require("utils.csv")

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

function translator.printTranslations()
    for k,v in pairs(translator.translations) do
      printTable(v)
      print("*****************************************************")
    end
end

function tr(str)
   local str = (translator.currentTranslation[str] or str):gsub("\\n", "\n")
   return str
end

return translator
