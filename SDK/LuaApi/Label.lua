dofile("Widget.lua")

--- The Label class
-- The Label class contains primitives to display and configure
-- UI based text labels. You can retrieve and change the current text data by
-- using the "setLabel" and "getLabel" functions.
--
-- See also: <a href="Widget.lua.html">Widget</a>
--
-- <strong>Example:</strong>
--
-- /code
-- -- Create a label
-- label = Label(10, 10, 100, 20, "This is an input field!")
--
-- -- Add it to the main canvas
-- mainCanvas:addWidget(label)
-- /endcode

--- Label(x,y,w,h,text)
-- Creates a new button.
--
-- @param x The x position on the canvas
-- @param y The y position on the canvas
-- @param w The width of the widget
-- @param h The height of the widget
-- @param text The label of the widget
Label = class(
	Widget,

	function(object, x, y, w, h, text)
		if text == nil then text = "" end
		object.widget = createLabel(x,y,w,h,text)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
		object.visible = true
	end)
