dofile("Widget.lua")

--- The Sprite class
-- The sprite class contains primitives to load, display and
-- manipulate sprites.
--
-- See also: <a href="Widget.lua.html">Widget</a>
--
-- <strong>Example:</strong>
--
-- /code
--  dofile("SDK/Graphics2D.lua")
--
--  -- Load a sprite without caption
--  sprite = Sprite(20,20,90,90,"maps/sprite.png", "")
--
--  -- Add sprite to the main canvas
--  mainCanvas:addWidget(sprite)
--
--  function onSceneUpdate()
--    -- Rotate sprite by 2° every frame
--    sprite:rotate(2)
--  end
-- /endcode

--- Sprite(x,y,w,h,file,text)
-- Loads a texture into memory and creates a Sprite object with it.
-- @param x The x position on the canvas
-- @param y The y position on the canvas
-- @param w The width of the widget
-- @param h The height of the widget
-- @param file The image file to load
-- @param text (optional) The label of the widget
Sprite = class(
	Widget,

	function(object, x, y, w, h, file, text)
		if text == nil then text = "" end
		object.widget = createSprite(x,y,w,h,file,text)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
		object.visible = true

		local size = getSpriteSize(object.widget)
		object.textureWidth = size[1]
		object.textureHeight = size[2]
end)
