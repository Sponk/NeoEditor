dofile("class.lua")
dofile("BatchSprite.lua")

--- SpriteBatch()
-- The SpriteBatch class implements an interface for fast rendering of
-- 2D sprites.
--
-- <strong>Example:</strong>
--
-- /code
-- local spriteBatch = SpriteBatch()
-- mainCanvas:addBatch(spriteBatch)
--
-- local sprite = spriteBatch:createSprite("maps/neo-icon.png")
-- sprite:translate(100, 0)
-- sprite:update()
-- /endcode
SpriteBatch = class(
   function(obj)
	  obj.batch = createSpriteBatch()
	  obj.sprites = {}
   end
)

--- Creates a new BatchSprite and adds it to the sprite batch.
-- @param filename The image file to use.
-- @return The new SpriteBatch object
function SpriteBatch:createSprite(filename)
   local sprite = BatchSprite(0,0,1,1,addSpriteToBatch(self.batch, filename))
   table.insert(self.sprites, sprite)
   return sprite
end
