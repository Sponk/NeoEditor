--- csv-test.lua - Test the CSV parser
local csv = dofile("../../utils/csv.lua")

function readTextTest()
   local text = csv.readText("somepath");
   assertEquals(nil, text)

   local text = csv.readText("assets/main.lua")
   assertNotEquals(nil, text)
end

function parseStringTest()
   local str = "key,value\nkey1,value1\nkey2,value2"
   local tbl = csv.parseString(str, ",")

   assertArrayEquals(
	  {
		 ["key"] = "value",
		 ["key1"] = "value1",
		 ["key2"] = "value2"
	  },
	  tbl)
end

tllut:addSuite({
	  ["readTextTest"] = readTextTest,
	  ["parseStringTest"] = parseStringTest
})
