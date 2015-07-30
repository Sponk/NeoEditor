-- main.lua - The main file of the NeoEditor
require("NeoLua")

dofile("utils/utils.lua")
dofile("utils/translator.lua")
dofile("utils/gui.lua")
dofile("utils/neo-extensions.lua")
dofile("settings.lua")

local runTests = true
local testUi = true

NeoLua.updateEasyAccess()

if testUi then
   NeoLua.engine:loadLevel(Settings.startLevel)
   dofile("editor/tests/generate-test-ui.lua")

   Gui.messageBox("Title", "THIS IS A MESSAGE")
end

if runTests then
   dofile("editor/tests/tests.lua")
end

-- Load the actual game
dofile("game.lua")
local gameUpdate = update

--- The update function for the editor
-- Calls the game update if the game is running
function update(dt)
   NeoLua.updateEasyAccess()
   
   if Settings.gameRunning and gameUpdate ~= nil then
	  gameUpdate(dt)
   end
end
