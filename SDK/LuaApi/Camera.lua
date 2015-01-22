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
	local point = Vector2.create()
	self:getScreenXY(obj,point);
	if(point:getX() + W1 > 0) and (point:getX() < W2) and (point:getY() + H1 > 0) and (point:getY() < H2) then 
		obj:setVisible(false)
		else 
			obj:setVisible(true)
	end
end

function Camera:getScreenXY(Obj,Point)
	if(Point == nil) then 
		Point = Vector2.create();
	end
	local x = Obj:getPosition().x
	local y = Obj:getPosition().y
	Point:setX(Point:getX() - self:getPosition().x)
	Point:setY(Point:getY() - self:getPosition().y)
	if Point:getX() > 0 then 
		Point:setX(Point:getX()+0.0000001)
	else 
		Point:setX(Point:getX()-0.0000001)
	end
	if Point:getY() > 0 then 
		Point:setY(Point:getY()+0.0000001)
	else 
		Point:setY(Point:getY()-0.0000001)
	end
	return Point;
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