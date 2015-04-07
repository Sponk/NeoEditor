dofile("Button.lua")

--- The ThemedButton class
-- The ThemedButton class contains primitives to display and configure
-- UI based buttons. You can specify a callback that will be called when the button is pressed.
-- This version of a button is themable, i.e. it loads sprites and displays them
-- instead of using plain colors.
--
-- See also: <a href="Widget.lua.html">Widget</a>
-- See also: <a href="Button.lua.html">Button</a>
--
-- Example:
-- /code
-- -- The button callback
-- function button_callback()
--     print("Button pressed!")
-- end
-- 
-- -- Create a button
-- button = ThemedButton(10, 10, 100, 20, "This is a button!", "button_callback")
-- 
-- -- Add it to the main canvas
-- mainCanvas:addWidget(button)
-- /endcode

--- ThemedButton(x,y,w,h,text,callback)
-- Creates a new button.
-- @param x The x position on the canvas
-- @param y The y position on the canvas
-- @param w The width of the widget
-- @param h The height of the widget
-- @param text The label of the widget
-- @param callback (optional) The callback of the button
ThemedButton = class(
	Button,

	function(object, x, y, w, h, text, callback)
		if text == nil then text = "" end
		object.widget = createThemedButton(x,y,w,h,text,callback)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
		object.visible = true
	end)
