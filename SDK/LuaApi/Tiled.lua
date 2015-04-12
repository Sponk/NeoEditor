--- The Tiled Loader
-- This file contains primitives to load and process Tiled maps.
-- Loading them currently allows only one layer and sprite sheet
-- for simplicity reasons.

TiledLayer = class(
   function(obj, t)
	  obj.type = t
	  obj.objects = {}
   end
)

TiledLevel = class(
   function(obj)
	  obj.path = ""
	  obj.layers = {}
   end
)

--- Converts a Tiled GID to a x/y position on the TileSheet.
-- @param gid The GID to convert
-- @param width The TileSheet width
-- @param height The TileSheet height
-- @return A vec2 containing the 2D coordinates.
function TiledLevel:getTilePosition(gid, width, height)

   local pos = {x = 0, y = 0}
   if gid == 0 then return {x = 0, y = -1} end

   gid = gid - 1

   pos.x = gid % width
   pos.y = math.floor(gid / width)
   return pos
end


--- Loads a Lua file exported by the Tiled 2D map editor
-- and returns a table of Tiles containing all loaded tiles.
-- @param canvas The canvas that will display the tiles
-- @param path The Lua file to load
function TiledLevel:loadTiledFile(path)

   path = path:gsub("/", ".")

   -- FIXME: Deletes every ".lua", not just the file ending!
   path = path:gsub(".lua", "")

   local tiledLevel = require(path)
   
   local resolution = getWindowScale()
   local tiledSpriteSheet = tiledLevel.tilesets[1]

   
   print("Found level version: " .. tiledLevel.version)
   print("Loading sprite sheet: " .. tiledSpriteSheet.image)
   local spriteSheet = SpriteSheet(tiledSpriteSheet.image, 
								   tiledSpriteSheet.tilewidth, 
								   tiledSpriteSheet.tileheight,
								   tiledSpriteSheet.spacing)

   for n,l in pairs(tiledLevel.layers) do
	  local layer = TiledLayer(l.type)
	  table.insert(self.layers, layer)

	  local x = 0
	  local y = 0
	  local tpos = {}

	  if l.type == "tilelayer" then
		 for i = 1, #l.data, 1 do
			
			x = x + 1
			if x >= l.width then
			   x = 0
			   y = y+1
			end
			
			tpos = self:getTilePosition(l.data[i], l.width, l.height)
			
			if tpos.x ~= -1 and tpos.y ~= -1 then   
			   table.insert(layer.objects, Tile(x*tiledSpriteSheet.tilewidth, 
												resolution[2]*0.5+y*tiledSpriteSheet.tileheight, 
												tiledSpriteSheet.tilewidth, 
												tiledSpriteSheet.tileheight,
												spriteSheet, tpos.x, tpos.y, ""))
			   
			   canvas:addWidget(layer.objects[#layer.objects])
			end
		 end
			
	  elseif l.type == "objectgroup" then
		 for n,o in pairs(l.objects) do	
			
			local tpos = self:getTilePosition(o.gid, tiledLevel.width, tiledLevel.height)
			table.insert(layer.objects, Tile(o.x,o.y,o.width,o.height,
											 spriteSheet, tpos.x, tpos.y, ""))
			
			canvas:addWidget(layer.objects[#layer.objects])
		 end
	  end
   end
end

--- Loads a Lua file exported by the Tiled 2D map editor
-- @param canvas The canvas that will display the tiles
-- @param path The Lua file to load
function loadTiledFile(canvas, path)

   path:gsub("/", ".")

   -- FIXME: Deletes every ".lua", not just the file ending!
   path:gsub(".lua", "")

   local tiledLevel = require(path)
   
   local resolution = getWindowScale()
   local tiledSpriteSheet = tiledLevel.tilesets[1]

   
   print("Found level version: " .. tiledLevel.version)
   print("Loading sprite sheet: " .. tiledSpriteSheet.image)
   local spriteSheet = SpriteSheet(tiledSpriteSheet.image, 
								   tiledSpriteSheet.tilewidth, 
								   tiledSpriteSheet.tileheight,
								   tiledSpriteSheet.spacing)
   
   local layer1 = tiledLevel.layers[1]
   local tiles = {}
   
   local x = 0
   local y = 0
   
   local tpos = {}
   
   for i = 1, #layer1.data, 1 do
	  
      x = x + 1
      if x >= layer1.width then
		 x = 0
		 y = y+1
      end
      
      tpos = getTilePosition(layer1.data[i], layer1.width, layer1.height)
      
      if tpos.x ~= -1 and tpos.y ~= -1 then   
		 tiles[#tiles+1] = Tile(x*tiledSpriteSheet.tilewidth, 
								resolution[2]*0.5+y*tiledSpriteSheet.tileheight, 
								tiledSpriteSheet.tilewidth, 
								tiledSpriteSheet.tileheight,
								spriteSheet, tpos.x, tpos.y, "")
		 
		 canvas:addWidget(tiles[#tiles])
      end
   end
   
   return tiles   
end
