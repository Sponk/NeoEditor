dofile("Widget.lua")

--- The Button class
-- The Button class contains primitives to display and configure
-- UI based buttons. You can specify a callback that will be called when the button is pressed.
--
-- See also: <a href="Widget.lua.html">Widget</a>
--
-- Example:
-- /code
-- -- The button callback
-- function button_callback()
--     print("Button pressed!")
-- end
-- 
-- -- Create a button
-- button = Button(10, 10, 100, 20, "This is a button!", "button_callback")
-- 
-- -- Add it to the main canvas
-- mainCanvas:addWidget(button)
-- /endcode

--- Button(x,y,w,h,text,callback)
-- Creates a new button.
-- @param x The x position on the canvas
-- @param y The y position on the canvas
-- @param w The width of the widget
-- @param h The height of the widget
-- @param text The label of the widget
-- @param callback (optional) The callback of the button
Button = class(
	Widget,

	function(object, x, y, w, h, text, callback)
		if text == nil then text = "" end
		object.widget = createButton(x,y,w,h,text,callback)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
		object.visible = true
	end)
