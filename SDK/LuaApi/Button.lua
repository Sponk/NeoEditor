dofile("Widget.lua")

--- The Button class
-- The Button class contains primitives to display and configure
-- UI based buttons. You can specify a callback that will be called when the button is pressed.
-- Extends the Widget class.

--- Button(x,y,w,h,text,callback)
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
--
-- callback: (optional) The callback of the button
Button = class(
	Widget,

	function(object, x, y, w, h, text, callback)
		if text == nil then text = "" end
		object.widget = createButton(x,y,w,h,text,callback)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
	end)
