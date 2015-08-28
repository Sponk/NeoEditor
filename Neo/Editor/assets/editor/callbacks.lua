--- callbacks.lua - Contains all menu callbacks for the editor
local LuaCommonDlg = require("LuaCommonDlg")

function saveCallback()
	local filename = LuaCommonDlg.getSaveFilename(os.getenv("HOME"))
	Gui.messageBox("Saving file", filename)
end

function openCallback()
	local filename = LuaCommonDlg.getOpenFilename(os.getenv("HOME"))

	if filename == nil then return end

	NeoLua.engine:loadLevel(filename)
	Editor.requestReload = true

	Gui.messageBox("Opening file", filename)
end

function lol() infoLog("lol") end

Shortcuts.addShortcut({"LCONTROL", "O"}, openCallback)
Shortcuts.addShortcut({"RCONTROL", "O"}, openCallback)

Shortcuts.addShortcut({"LCONTROL", "S"}, saveCallback)
Shortcuts.addShortcut({"RCONTROL", "S"}, saveCallback)
