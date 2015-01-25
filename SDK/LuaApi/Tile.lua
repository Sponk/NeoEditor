dofile("Widget.lua")

--- The Tile class
-- The tile class contains primitives to display the contents of a SpriteSheet.
--
-- See also: <a href="Widget.lua.html">Widget</a>
--
-- Example:
--[[
  dofile("SDK/Graphics2D.lua")

  -- Load a spritesheet
  spritesheet = SpriteSheet("maps/anim.png",90,90,2)

  -- Create a looping animation
  -- from 0 to 3 with 70ms per frame
  animation = SpriteAnimation(0,3,70,true)

  -- Create a tile without caption
  tile = Tile(20,20,90,90,spritesheet, 0, 0, "")

  -- Attach animation to tile
  tile:attachAnimation(animation)

  -- Add tile to the main canvas
  mainCanvas:addWidget(tile)

  function onSceneUpdate()
    -- Update tile animation
    tile:update()
  end
]]
-- See also: <a href="SpriteSheet.lua.html">SpriteSheet</a>,
-- <a href="SpriteAnimation.lua.html">SpriteAnimation</a>

--- Tile(x,y,w,h,spritesheet,tx,ty,text)
-- Constructs a Tile object. The text parameter is optional.
Tile = class(
	Widget,

	function(object, x, y, w, h, spritesheet,tx,ty,text)
		if text == nil then text = "" end
		object.widget = createTile(x,y,w,h,text,tx,ty)

		setTileSpriteSheet(object.widget, spritesheet.spriteSheet)

		object.position = {x,y}
		object.width = w
		object.height = h
		object.rotation = 0
		object.spritesheet = spritesheet
		object.tx = tx
		object.ty = ty
		object.animation = nil
		object.visible = true
	end)

--- Attach an SpriteAnimation to an Tile object
-- and the animation will automatically apply when calling
-- '#Tile:update' every frame.
--
-- animation: The SpriteAnimation object to attach.
function Tile:attachAnimation(animation)
    self.animation = animation
end

--- Updates the attached animation if there is one.
function Tile:update()
    if self.animation ~= nil then
	-- TODO: Transform to right coordinate in sprite sheet!
	local frame = self.animation:nextFrame()
	setTileOffset(self.widget, {frame, self.ty})
    end
end
