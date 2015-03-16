setPluginName("Images as Planes")
setPluginAuthor("Yannick Pflanzer")
setPluginDescription("A plugin that allows you to load a 2D image into a 3D scene.")

addEditorMenu("Import image", "load_callback")

require("plugins.libs.misc")

-- Finds the last occurance of fmt in the given string.
--
-- str: The string to search in
-- fmt: The pattern to search for
--
function find_last_of(str, fmt)
	local rev = str:reverse()
	
	-- Add one because Lua indices start at one and not at zero
	return str:len() - rev:find(fmt) + 1
end

-- This function is called, when the menu item gets clicked.
-- It creates a meshfile and assigns the image as texture to it.
-- Then it loads the new mesh into the scene.
--
-- TODO: Scale the mesh to fit the w/h ratio of the texture!
function load_callback()

	local projectDir = getProjectDir()
	if projectDir:len() == 0 then
		messagebox("You have to load a project first!")
		return
	end
	
	-- Find all required filenames
	local filename = openFileDlg("Choose a file", getProjectDir() .. "maps", "*.png")

	local texture = createSprite(0,0,0,0,filename, "")
	local dimensions = getSpriteSize(texture)
	debugLog("Texture has dimensions of " .. dimensions[1] .. "x" .. dimensions[2])
	
	
	filename = relpath(filename, projectDir .. "meshs")
	debugLog("Loading image file: " .. filename)

	local idx = find_last_of(filename, "/")
	
	local meshname = projectDir .. "meshs" .. filename:sub(idx, filename:len()) .. ".mesh"
	debugLog("Saving to: " .. meshname)
	
	-- Generate the .mesh file
	local templateFile = io.open("plugins/template.mesh", "r")	
	local data = templateFile:read("*all")
	
	data = data:gsub("$IMAGE_FILENAME", filename)
	data = data:gsub("$IMAGE_WIDTH", dimensions[1])
	data = data:gsub("$IMAGE_HEIGHT", dimensions[2])
	
	local meshfile = io.open(meshname, "w")
	meshfile:write(data)
	meshfile:flush()
	meshfile:close()
	
	loadMesh(meshname)
	updateEditorView()
end
