dofile("Widget.lua")

--- The Label class
-- The Label class contains primitives to display and configure
-- UI based text labels. You can retrieve and change the current text data by
-- using the "setLabel" and "getLabel" functions.
-- Extends the Widget class.

--- Label(x,y,w,h,text)
-- Creates a new button.
--
-- x: The x position on the canvas
--
-- y: The y position on the canvas
--
-- w: The width of the widget
--
-- h: The height of the widget
--
-- text: The label of the widget
Label = class(
	Widget,

	function(object, x, y, w, h, text)
		if text == nil then text = "" end
		object.widget = createLabel(x,y,w,h,text)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
	end)
