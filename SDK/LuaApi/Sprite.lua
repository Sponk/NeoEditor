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

---If a bounding box is assigned then this function needs to be called
function Sprite:update()
	if self.boundingBox ~= nil then
		self:setPosition(self.boundingBox.x,self.boundingBox.y)
	end
end	
---Add a specific bounding box to this sprite.Ex: 'rect','circle','line'
--- It can be a rectangle,circle or a line
--- Tag - used for checking collision detection
--- Width - the width of the bounding box(it can be bigger or smaller than the width of the image)
--- Height - the height of the bounding box(it can be bigger or smaller than the height of the image)
function Sprite:addBoundingBox(shape,tag,width,height)
	self.boundingBox = nil
	self.boundingBox = collision:addDynamic(shape,tag,self:getPosition().x,self:getPosition().y,width,height)
end	

function Sprite:getBoundingBox()
	return self.boundingBox
end

function Sprite:addFriction(value)
	if self.boundingBox ~= nil then
	self.boundingBox.friction = value
	end
end
function Sprite:addDamping(value)
	if self.boundingBox ~= nil then
		self.boundingBox.damping = value
	end
end

function Sprite:move(speedX,speedY)
	if self.boundingBox ~= nil then
	self.boundingBox.xv = self.boundingBox.xv + speedX
	self.boundingBox.yv = self.boundingBox.yv + speedY
	end
end

function Sprite:moveXBy(speedX)
	if self.boundingBox ~= nil then
	self.boundingBox.xv = self.boundingBox.xv + speedX
	end
end

function Sprite:moveYBy(speedY)
	if self.boundingBox ~= nil then
	self.boundingBox.yv = self.boundingBox.yv + speedY
	end
end

function Sprite:moveX(speedX)
	if self.boundingBox ~= nil then
		self.boundingBox.xv = speedX
	end
end

function Sprite:moveY(speedY)
	if self.boundingBox ~= nil then
	self.boundingBox.yv = speedY
	end
end
