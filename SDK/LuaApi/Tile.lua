dofile("Widget.lua")

--- The Tile class
-- The tile class contains primitives to display the contents of a SpriteSheet.
-- Extends the Widget class.

--- Tile(x,y,w,h,spritesheet,tx,ty,text)
-- Constructs a Tile object. The text parameter is optional.
Tile = class(
	Widget,

	function(object, x, y, w, h, spritesheet,tx,ty,text)
		if text == nil then text = "" end
		object.widget = createSprite(x,y,w,h,file,text)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
		object.spritesheet = spritesheet
		object.tx = tx
		object.ty = ty
	end)
