-- main.lua - The main file of the NeoEditor
require("NeoLua")

Translator = dofile("utils/translator.lua")

dofile("utils/utils.lua")
dofile("utils/gui.lua")
dofile("utils/neo-extensions.lua")
dofile("settings.lua")

local runTests = false
local testUi = true

if testUi then
   NeoLua.engine:loadLevel(Settings.startLevel)
   -- dofile("editor/tests/generate-test-ui.lua")
end

if runTests then
   dofile("editor/tests/tests.lua")
   os.exit(0)
end

if Settings.editorEnabled then
   dofile("editor/main.lua")
else
  -- Load the actual game if the editor is not running
  dofile("game.lua")
end

-- Put the custon update into a variable
-- for later use
local gameUpdate = update

--- The update function for the editor
-- Calls the game update if the game is running
-- or the editor update if the editor is loaded
function update(dt)
   NeoLua.updateEasyAccess()
   
   if Settings.gameRunning and gameUpdate ~= nil then
	  pcall(gameUpdate, dt)
   end
end
