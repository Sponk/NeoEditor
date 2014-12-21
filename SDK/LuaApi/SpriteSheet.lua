dofile("class.lua")

--- The SpriteSheet class
-- Contains the primitives to load and use a sprite sheet.

--- SpriteSheet(file,tilweidth,tileheight,spacing)
-- Loads a sprite sheet and creates a SpriteSheet object.
SpriteSheet = class(
	function(object, file, tilewidth, tileheight, spacing)
	    object.spriteSheet = loadSpriteSheet(file, tilewidth, tileheight, spacing)
	end)
