--- csv.lua - A collection of tools to read and parse CSV files into Lua tables

local utf8 = require("lua-utf8")
local csv = {}

--- Reads the content of a text file and returns it as a string
-- @param path The path of the file to load
-- @return The content of the file as a string
csv.readText = function(path)
   local f = io.open(path, "r")

   if f == nil then
	  print("Text file " .. path .. " could not be opened for reading!")
	  return nil
   end

   return f:read("*all")
end

--- Reads a file and parses the CSV content.
-- @param path The path to the file to load
-- @param delim The delimiter to use
-- @return A table with all key value pairs
function csv.parseFile(path, delim)   
   local text = csv.readText(path)
   return csv.parseString(text, delim)
end

--- Parses a CSV string and returns a table of key value pairs.
-- @param str The string to parse
-- @param delim The delimiter to use
-- @return The table with the key value pairs
function csv.parseString(str, delim)
   local retval = {}
   
   -- old: "(%w+)" .. delim .. "(%w+)"
   local pattern = "([%w_ :]+)" .. delim .. "([%w_ :%\\%p]+)"
   local lines = utf8.gmatch(str, "([^\n]+)")
   for k,v in lines do
	   local values = utf8.gmatch(str, pattern)
  	 for i,j in values do
  		retval[i] = j
  	 end
   end
   
   return retval
end

return csv
