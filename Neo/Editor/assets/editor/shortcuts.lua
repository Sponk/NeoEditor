 --- shortcuts.lua - Contains functions to implement keyboard shortcuts.
 -- Configurable by setting callbacks for specific keypresses

local keymap = {}
local shortcuts = {}
local handled = {}

function shortcuts.update()

	local input = NeoLua.input

	for k,v in pairs(keymap) do
		local triggered = true
		for i,j in ipairs(k) do
			if not input:isKeyPressed(j) then
				triggered = false
			end
		end

		if triggered and not handled[k] then v() end

		handled[k] = triggered
	end
end

function shortcuts.addShortcut(key, callback)
	if type(key) == "string" then
		key = {key}
	end

	keymap[key] = callback
end

return shortcuts
