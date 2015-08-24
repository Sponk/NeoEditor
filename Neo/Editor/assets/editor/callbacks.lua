--- callbacks.lua - Contains all menu callbacks for the editor
local LuaCommonDlg = require("LuaCommonDlg")

function saveCallback()
	local filename = LuaCommonDlg.getSaveFilename(os.getenv("HOME"))
	Gui.messageBox("Saving file", filename)
end

function openCallback()
	local filename = LuaCommonDlg.getOpenFilename(os.getenv("HOME"))

	if filename == nil then return end

	Gui.messageBox("Opening file", filename)
	NeoLua.engine:loadLevel(filename)

	Editor.setupLevel()

	--Editor.select(nil)
	--Editor.entityEditor.setShownObject(nil)
	Editor.updateSceneTree()
end

function lol() infoLog("lol") end

Shortcuts.addShortcut({"LCONTROL", "O"}, openCallback)
Shortcuts.addShortcut({"RCONTROL", "O"}, openCallback)

Shortcuts.addShortcut({"LCONTROL", "S"}, saveCallback)
Shortcuts.addShortcut({"RCONTROL", "S"}, saveCallback)
