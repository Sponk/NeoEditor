dofile("class.lua")

--- The BatchSprite class
-- This class contains primitives to load, display and
-- manipulate sprites that are managed using a SpriteBatch.
--
-- See also: <a href="SpriteBatch.lua.html">SpriteBatch</a>
--
-- <strong>Example:</strong>
--
-- /code
--  dofile("SDK/Graphics2D.lua")
--
--  -- Create a new sprite batch
--  batch = SpriteBatch()
--
--  -- Load a sprite without caption
--  sprite = BatchSprite(20,20,90,90,"maps/sprite.png")
--  
--  -- Add sprite to batch
--  batch:addSprite(sprite)
--
--  -- Add sprite batch to the main canvas
--  mainCanvas:addSpriteBatch(batch)
--
--  function onSceneUpdate()
--    -- Rotate sprite by 2° every frame
--    sprite:rotate(2)
--  end
-- /endcode

--- SpriteBatch(x,y,w,h,file)
-- Loads a texture into memory and creates a Sprite object with it.
-- @param x The x position on the canvas
-- @param y The y position on the canvas
-- @param w The width of the widget
-- @param h The height of the widget
-- @param nativeSprite A native BatchSprite object
BatchSprite = class(
	function(object, x, y, w, h, nativeSprite)
		object.widget = nativeSprite --createBatchSprite(x,y,w,h,file)
		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
		object.visible = true
end)

--- Sets the current position of the widget
-- @param x The new X position
-- @param y The new Y position
function BatchSprite:setPosition(x,y)
    self.position[1] = x
    self.position[2] = y
end

--- Sets the current rotation of the widget
-- @param r The new rotation angle
function BatchSprite:setRotation(r)
    self.rotation = r
end

--- Rotates the widget by an angle.
-- @param angle The angle to rotate by
function BatchSprite:rotate(angle)
    self:setRotation(self.rotation + angle)
end

--- Translates the widget by the given coordinates relative to the current position.
-- @param x The X offset
-- @param y The Y offset
function BatchSprite:translate(x,y)
    self:setPosition(self.position[1] + x, self.position[2] + y)
end


--- Gets the current position of the widget.
-- @param x The X cordonat
-- @param y The Y cordonat
function BatchSprite:getPosition()
    local pos = {}
    pos.x = self.position[1]
    pos.y = self.position[2]
    return pos
end

--- Returns the width and height of this object.
-- @return A vec2 with the width and the height.
function BatchSprite:getSize()
    local size = {}
    size.width = self.width
    size.height = self.height
    return size
end

--- Pushes all data like position and rotation to the rendering
-- backend.
-- <strong>Attention:</strong> All changes to the sprite will only take effect
-- if this method is called!
function BatchSprite:update()
   updateBatchSprite(self.widget, self.position[1], self.position[2], 
					 self.width, self.height, self.rotation)
end
