Camera = {}
Camera.__index = Camera

---Pass the reference of the main camera in your game.Ex: "Camera01"
function Camera.create(c)
	local self = {}
	setmetatable(self, Camera)

	self.camera = Object3d.getObject(c)
	self.x = 0
	self.y = 0
	self.z = 0
	return self
end

---Checks to see if a certain sprite is on the screen.If it`s not then dont draw it
function Camera:isOnScreen(obj,W1,H1,W2,H2,W3,H3)
	local function CheckCollision(x1,y1,w1,h1, x2,y2,w2,h2)
		return x1 < x2+w2 and
			x2 < x1+w1 and
			y1 < y2+h2 and
			y2 < y1+h1
	end	
	local visible = true
	if CheckCollision(self:getPosition().x,self:getPosition().y,W1,H1,obj:getPosition().x,obj:getPosition().y,W2,H2) then
		obj:setVisible(true)
		visible = true
	else 
		if CheckCollision(self:getPosition().x,self:getPosition().y,W1,H1,obj:getPosition().x,obj:getPosition().y,W3,H3) then 
			obj:setVisible(true)
			visible = true
		else 
			obj:setVisible(false)
			visible = false
		end
	end
	return visible
end

--Move the camera
function Camera:translate(value)
	self.camera:translate(value)
end

--From the acctual position offset it X,Y axe
function Camera:offset(x,y)
	setGuiSystemCameraOffset(0,{x,y})
end
	
--Follow a certain sprite,tile 
function Camera:follow(obj)
	local X = obj:getPosition().x
	local Y = obj:getPosition().y

	self.camera:setPosition({X,Y,-1})
end

function Camera:getPosition()
	self.x = self.camera:getPosition()[1]
	self.y = self.camera:getPosition()[2]
	self.z = self.camera:getPosition()[3]
	
	local pos = {}

	pos.x = self.x 
	pos.y = self.y 
	pos.z = self.z

	return pos
end
