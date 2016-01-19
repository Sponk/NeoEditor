--- neo-extensions.lua - Contains helper functions that are registered to NeoLua
-- to extend its functionality.

--- Sets variables for the current engine contexts
-- (i.e. current level, current scene, current physics context etc.)
-- for easier access
function NeoLua.updateEasyAccess()
   NeoLua.engine = NeoLua.NeoEngine.getInstance()
   NeoLua.level = NeoLua.engine:getLevel()
   NeoLua.physics = NeoLua.engine:getPhysicsContext()
   NeoLua.input = NeoLua.engine:getInputContext()
   NeoLua.system = NeoLua.engine:getSystemContext()
end

function NeoLua.sleep(time)
  local endtime = NeoLua.system:getSystemTick() + time
  while NeoLua.system:getSystemTick() < endtime do end
end

NeoLua.updateEasyAccess()
