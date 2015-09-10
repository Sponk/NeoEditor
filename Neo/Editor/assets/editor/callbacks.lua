--- callbacks.lua - Contains all menu callbacks for the editor
local LuaCommonDlg = require("LuaCommonDlg")

function saveCallback()
	local filename = LuaCommonDlg.getSaveFilename(NeoLua.system:getWorkingDirectory() .. "/assets", "level")

	if filename ~= nil then
		Editor.cleanUp()
		NeoLua.engine:saveLevel(filename)

		Editor.setupLevel()
		Editor.loadMeshes()
	end

end

function openCallback()
	local filename = LuaCommonDlg.getOpenFilename(NeoLua.system:getWorkingDirectory() .. "/assets", "level")

	if filename == nil then return end

	if not NeoLua.engine:loadLevel(filename) then

	end

	Editor.requestReload = true
end

Shortcuts.addShortcut({"LCONTROL", "O"}, openCallback)
Shortcuts.addShortcut({"RCONTROL", "O"}, openCallback)

Shortcuts.addShortcut({"LCONTROL", "S"}, saveCallback)
Shortcuts.addShortcut({"RCONTROL", "S"}, saveCallback)

Shortcuts.addShortcut({"1"}, function()
	Editor.translationMode = Editor.sceneMeshes.translation
	Editor.select(nil)
end)

Shortcuts.addShortcut({"2"}, function()
	Editor.translationMode = Editor.sceneMeshes.rotation
	Editor.select(nil)
end)

Shortcuts.addShortcut({"3"}, function()
	Editor.translationMode = Editor.sceneMeshes.scale
	Editor.select(nil)
end)