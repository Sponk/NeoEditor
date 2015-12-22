--- callbacks.lua - Contains all menu callbacks for the editor
local LuaCommonDlg = {} --require("LuaCommonDlg")
-- dofile("dialogs/filedlg.lua")

local function appendLevelExtension(filename)
	if filename:sub(1, -string.len(".level")) ~= ".level" then
		return filename .. ".level"
	end

	return filename
end

function saveCallback()
	local filename = Editor.project.level or LuaCommonDlg.getSaveFilename(NeoLua.system:getWorkingDirectory() .. "/assets", "level")

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

function saveAsCallback()
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

function filedlg_openLevelCallback()
	infoLog("Loading level: " .. Editor.openFileDlg:getSelectedFilename());
	local filename = Editor.openFileDlg:getSelectedFilename()

	if filename == nil or filename == "" then return end

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

function openCallback()
	Editor.openFileDlg:setScriptCallback("filedlg_openLevelCallback")
	Editor.openFileDlg:setVisible(true)
	Editor.openFileDlg:setFilter(".*\\.level")
	Editor.openFileDlg:readDirectory(NeoLua.system:getWorkingDirectory() .. "/assets");
end

function addLightCallback()
	Editor.addLight()
end

function filedlg_openMeshCallback()
	local filename = Editor.openFileDlg:getSelectedFilename()
	if filename == nil or filename == "" then return end
	Editor.addEntity(filename)
end

function filedlg_openSoundCallback()
	local filename = Editor.openFileDlg:getSelectedFilename()
	if filename == nil or filename == "" then return end
	Editor.addSound(filename)
end

function filedlg_openTextCallback()
	local filename = Editor.openFileDlg:getSelectedFilename()
	if filename == nil or filename == "" then return end
	Editor.addText(filename)
end

function addEntityCallback()
	Editor.openFileDlg:setScriptCallback("filedlg_openMeshCallback")
	Editor.openFileDlg:setVisible(true)
	Editor.openFileDlg:setFilter("(.*\\.mesh)|(.*\\.obj)|(.*\\.dae)|(.*\\.3ds)|(.*\\.fbx)")
	Editor.openFileDlg:readDirectory(NeoLua.system:getWorkingDirectory() .. "/assets");
end

function addSoundCallback()
	Editor.openFileDlg:setScriptCallback("filedlg_openSoundCallback")
	Editor.openFileDlg:setVisible(true)
	Editor.openFileDlg:setFilter("(.*\\.ogg)|(.*\\.wav)")
	Editor.openFileDlg:readDirectory(NeoLua.system:getWorkingDirectory() .. "/assets");
end

function addTextCallback()
	Editor.openFileDlg:setScriptCallback("filedlg_openTextCallback")
	Editor.openFileDlg:setVisible(true)
	Editor.openFileDlg:setFilter(".*\\.ttf")
	Editor.openFileDlg:readDirectory(NeoLua.system:getWorkingDirectory() .. "/assets");
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