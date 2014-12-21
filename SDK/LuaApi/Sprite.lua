dofile("Widget.lua")

--- The Sprite class
-- The sprite class contains primitives to load, display and
-- manipulate sprites. Extends the Widget class.

--- Sprite(x,y,w,h,file,text)
-- Loads a texture into memory and creates a Sprite object with it.
--
-- x: The x position on the canvas
--
-- y: The y position on the canvas
--
-- w: The width of the widget
--
-- h: The height of the widget
--
-- file: The image file to load
--
-- text: (optional) The label of the widget
Sprite = class(
	Widget,

	function(object, x, y, w, h, file, text)
		if text == nil then text = "" end
		object.widget = createSprite(x,y,w,h,file,text)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
	end)
