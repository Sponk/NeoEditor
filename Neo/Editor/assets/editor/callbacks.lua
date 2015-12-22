--- callbacks.lua - Contains all menu callbacks for the editor
local LuaCommonDlg = {} --require("LuaCommonDlg")
-- dofile("dialogs/filedlg.lua")

local function appendLevelExtension(filename, extension)
	if filename:sub(1, -string.len(extension)) ~= extension then
		return filename .. extension
	end

	return filename
end

function filedlg_saveLevelCallback()
	infoLog("Saving level: " .. Editor.saveFileDlg:getSelectedFilename());
	local filename = Editor.saveFileDlg:getSelectedFilename()

	if filename == nil or filename == "" then return end

	filename = appendLevelExtension(filename, ".level")

	Editor.cleanUp()
	NeoLua.engine:saveLevel(filename)

	Editor.setupLevel()
	Editor.loadMeshes()

	NeoLua.system:setWindowTitle("Neo Scene Editor - " .. filename)
	Editor.project.level = filename
end

function saveCallback()
	if Editor.project.level == nil then
		saveAsCallback()
	else
		Editor.cleanUp()
		NeoLua.engine:saveLevel(Editor.project.level)

		Editor.setupLevel()
		Editor.loadMeshes()

		NeoLua.system:setWindowTitle("Neo Scene Editor - " .. Editor.project.level)
	end
end

function saveAsCallback()
	Editor.saveFileDlg:setScriptCallback("filedlg_saveLevelCallback")
	Editor.saveFileDlg:setVisible(true)
	Editor.saveFileDlg:setFilter(".*\\.level")
	Editor.saveFileDlg:readDirectory(NeoLua.system:getWorkingDirectory() .. "/assets");
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