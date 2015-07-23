--- utils.lua - Contains some utilities for doing common work

function printTable(t)
  for k,v in pairs(t) do
    print(k,v)
  end
end

function infoLog(msg)
  local info = debug.getinfo(2) 
  NeoLua.infoLog(tostring(msg), (info.name or info.what), info.filename, info.currentline)
end