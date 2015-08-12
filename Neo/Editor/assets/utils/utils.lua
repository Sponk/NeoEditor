--- utils.lua - Contains some utilities for doing common work
local utf8 = require("lua-utf8")

function printTable(t)
  for k,v in pairs(t) do
    print(k,v)
  end
end

function infoLog(msg)
  local info = debug.getinfo(2) 
  NeoLua.infoLog(tostring(msg), (info.name or info.what), info.filename, info.currentline)
end

function round(num, decimals)
  local multiplier = 10^(decimals or 0)
  return math.floor(num * multiplier + 0.5) / multiplier
end

function findName(stem, removeNumbers)

  local pattern = "(%a+)"
  if removeNumbers == nil or removeNumbers == false then
    pattern = ".+"
  end

  local namePart = utf8.match(stem, pattern)
  local scene = NeoLua.level:getCurrentScene()
  
  local i = 0
  local name = namePart
  while scene:getObjectByName(name) ~= nil do
    name = namePart .. i  
    i = i + 1
  end
  
  return name
end