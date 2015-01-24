Vector2 = {}
Vector2.__index = Vector2

function Vector2.create(x,y)
	local self = {}
	setmetatable(self,Vector2)
	self.x = x or 0 
	self.y = y or 0
	return self
end

function Vector2:mul(v)
	return Vector2.create(self.x*v:getX(),self.y*v:getY());
end

function Vector2:div(v)
	return Vector2.create(self.x/v:getX(),self.y/v:getY());
end

function Vector2:sum(v)
	return Vector2.create(self.x+v:getX(),self.y+v:getY());
end

function Vector2:sub(v)
	return Vector2.create(self.x-v:getX(),self.y-v:getY());
end

function Vector2:addX(x)
	self.x = self.x + x
end

function Vector2:addY(y)
	self.y = self.y + y
end

function Vector2:set(x,y)
	self.x = x 
	self.y = y
end

function Vector2:setX(x)
	self.x = x 
end

function Vector2:setY(y)
	self.y = y 
end

function Vector2:getX()
	return self.x 
end

function Vector2:getY()
	return self.y 
end

function Vector2:get()
	local c = {}
	c.x = self.x 
	c.y = self.y 
	return c
end

function Vector2:normalise()
	local c = {}
	c.x = self.x / self.x 
	c.y =  self.y / self.y 
	return c 
end