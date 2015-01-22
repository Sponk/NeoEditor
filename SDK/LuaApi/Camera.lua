Camera = {}
Camera.__index = Camera


function Camera.create(c)
	local self = {}
	setmetatable(self, Camera)

	self.camera = Object3d.getObject(c)
	self.x = 0
	self.y = 0
	self.z = 0
	return self
end

function Camera:isOnScreen(obj,W1,H1,W2,H2)
	 local x = self:getPosition().x
	 local y = self:getPosition().y
	 local X = obj:getPosition().x
	 local Y = obj:getPosition().y
	
	 
     if not(X + W1 < x or X > x + W2 or 
			Y + H1 < y or Y > y + H2) then 
		obj:setVisible(false)
	end
	 if (X+12<0) then 
			obj:setVisible(false)
		end
end

function Camera:follow(obj,offsetX,offsetY)
	self.x = self.camera:getPosition()[1]
	self.y = self.camera:getPosition()[2]
	self.z = self.camera:getPosition()[3]
	
	local X = obj:getPosition().x + (offsetX or 0)
	local Y = obj:getPosition().y + (offsetY or 0)

	self.camera:setPosition({X,Y,0})
end

function Camera:getPosition()
	local pos = {}

	pos.x = self.x 
	pos.y = self.y 
	pos.z = self.z

	return pos
end