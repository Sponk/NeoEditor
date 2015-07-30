--- translation-test.lua - Tests the translator

local translator = dofile("../../utils/translator.lua")

function translatorTest()
   local str = "key,value\nkey1,value1\nkey2,value2"
   local str2 = "key,wert\nkey1,wert1\nkey2,wert2"

   translator.addTranslationString("test1", str)
   translator.addTranslationString("test2", str2)

   assertEquals("wert", tr("key"))

   translator.swapTranslation("test1")
   assertEquals("value", tr("key"))   
end

tllut:addSuite({
	  ["translatorTest"] = translatorTest
})

