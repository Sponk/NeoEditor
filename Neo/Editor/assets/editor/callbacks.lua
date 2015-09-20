--- callbacks.lua - Contains all menu callbacks for the editor
local LuaCommonDlg = require("LuaCommonDlg")

local function appendLevelExtension(filename)
	if filename:sub(1, -string.len(".level")) ~= ".level" then
		return filename .. ".level"
	end

	return filename
end

function saveCallback()
	local filename = LuaCommonDlg.getSaveFilename(NeoLua.system:getWorkingDirectory() .. "/assets", "level")

	if filename ~= nil then

		filename = appendLevelExtension(filename)

		Editor.cleanUp()
		NeoLua.engine:saveLevel(filename)

		Editor.setupLevel()
		Editor.loadMeshes()

		NeoLua.system:setWindowTitle("Neo Scene Editor - " .. filename)
		Editor.project.level = filename
	end
end

function openCallback()
	local filename = LuaCommonDlg.getOpenFilename(NeoLua.system:getWorkingDirectory() .. "/assets", "level")

	if filename == nil then return end

	if not NeoLua.engine:loadLevel(filename) then
		infoLog("Could not load level " .. filename)
		Editor.project.level = ""

		NeoLua.system:setWindowTitle("Neo Scene Editor")
		Editor.requestReload = true

		return
	end

	NeoLua.system:setWindowTitle("Neo Scene Editor - " .. filename)
	Editor.requestReload = true
end

Shortcuts.addShortcut({"LCONTROL", "O"}, openCallback)
Shortcuts.addShortcut({"RCONTROL", "O"}, openCallback)

Shortcuts.addShortcut({"LCONTROL", "S"}, saveCallback)
Shortcuts.addShortcut({"RCONTROL", "S"}, saveCallback)

local function enableHandles(h, e)
	for k,v in pairs(h) do
		v:setActive(e)
	end
end

Shortcuts.addShortcut({"1"}, function()
	enableHandles(Editor.translationMode, false)
	Editor.translationMode = Editor.sceneMeshes.translation
	enableHandles(Editor.translationMode, true)

	--Editor.select(nil)
end)

Shortcuts.addShortcut({"2"}, function()
	enableHandles(Editor.translationMode, false)
	Editor.translationMode = Editor.sceneMeshes.rotation
	enableHandles(Editor.translationMode, true)

	--Editor.select(nil)
end)

Shortcuts.addShortcut({"3"}, function()
	enableHandles(Editor.translationMode, false)
	Editor.translationMode = Editor.sceneMeshes.scale
	enableHandles(Editor.translationMode, true)

	--Editor.select(nil)
end)