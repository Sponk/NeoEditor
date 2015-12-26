--- addonsystem.lua - Contains the addon system.
-- Allows loading, unloading and managing addons written in Lua script.

local lfs = require("lfs")
local addonsystem = {
	addonFiles = {}
}

function addonsystem.loadAddonsFrom(dir)
	infoLog("Loading addons from " .. dir)

	local iter, dir_obj = lfs.dir(dir)
	local v = iter(dir_obj)

	local addonsList = ""
	while v ~= nil do
		if v:find(".lua") ~= nil then
			local env = {AddonSystem = addonsystem, infoLog = infoLog, NeoLua = NeoLua, math = math,
							Gui = Gui, Shortcuts = Shortcuts, tr = tr, tostring = tostring, Editor = Editor, type = type,
							round = round, tonumber = tonumber}
			local func = loadfile(dir .. "/" .. v)

			-- DEPRECATED
			setfenv(func, env)
			local ret, val = pcall(func)

			if not ret then
				infoLog("Could not load addon: " .. val)
			else
				infoLog("Loaded plugin \"" .. tostring(val.name) .. "\" from " .. v)
				addonsystem.addonFiles[val.name] = {addon = val, env = env, file = dir .. "/" .. v}
				addonsList = addonsList .. v .. " "
			end
		end

		v = iter(dir_obj)
	end

	infoLog("Found editor addons: " .. addonsList)
end

function addonsystem.onBegin(addon)
	if addon.env and addon.addon then
		setfenv(addon.addon.onBegin, addon.env)
		ret, err = pcall(addon.addon.onBegin)
		if not ret then
			infoLog("Could not initialize addon: " .. err)
			return false
		end
	end

	return true
end

function addonsystem.onEnd(addon)
	if addon.env and addon.addon then
		setfenv(addon.addon.onEnd, addon.env)
		ret, err = pcall(addon.addon.onEnd)
		if not ret then
			infoLog("Could not end addon: " .. err)
			return false
		end
	end

	return true
end

function addonsystem.endAll()
	for k,v in pairs(addonsystem.addonFiles) do
		addonsystem.onEnd(v)
	end
end

function addonsystem.beginAll()
	for k,v in pairs(addonsystem.addonFiles) do
		addonsystem.onBegin(v)
	end
end

local numcallbacks = 0
function addonsystem.registerCallback(addonName, fn)

	if not addonName or not fn then error("Invalid arguments given!") end

	local name = "EditorAddonCallback" .. numcallbacks
	_G[name] = fn

	setfenv(fn, addonsystem.addonFiles[addonName].env)
	numcallbacks = numcallbacks + 1
	return name
end

return addonsystem

