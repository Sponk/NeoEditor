dofile("Widget.lua")

--- The Sprite class
-- The sprite class contains primitives to load, display and
-- manipulate sprites.
--
-- See also: <a href="Widget.lua.html">Widget</a>
--
-- Example:
--[[
  dofile("SDK/Graphics2D.lua")

  -- Load a sprite without caption
  sprite = Sprite(20,20,90,90,"maps/sprite.png", "")

  -- Add sprite to the main canvas
  mainCanvas:addWidget(sprite)

  function onSceneUpdate()
    -- Rotate sprite by 2° every frame
    sprite:rotate(2)
  end
]]

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
		object.visible = true
	end)
