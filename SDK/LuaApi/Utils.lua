--- Author: Murii

Utils = {}
Utils.__index = Utils

function Utils.create()
	local self = {}
	setmetatable(self, Utils)
 	
 	-- The value PI as a float. (180 degrees)
	self.PI = 3.1415927;

	self.RADTODEG = (180 / self.PI);
	self.DEGTORAD = (self.PI / 180);
	
	 -- The value 2PI as a float. (360 degrees)
	 self.TWO_PI = 2.0 * self.PI;
	 --The value PI/2 as a float. (90 degrees)
	 self.HALF_PI = 0.5 * self.PI;

	return self
end

--- Finds the length of the given vector
function Utils:vectorLength(dx,dy)
	return math.sqrt(dx * dx + dy * dy);
end

--- Finds the distance between two tables
function Utils:distanceBetween(a,b)
	local dx = a.x - b.x;
	local dy =  a.y - b.y;
	return (self:vectorLength(dx, dy));
end

function Utils:asDegrees(radians)
	return radians * self.RADTODEG;
end

function Utils:asRadians(degrees)
	return radians * self.DEGTORAD;
end

--- * Find the angle (in radians) between an table and an table. The source sprite takes its x/y and origin into account.
-- * The angle is calculated in clockwise positive direction (down = 90 degrees positive, right = 0 degrees positive, up = 90 degrees negative)
function Utils:angleBetweenPoint(a,target,asDegrees)
		local dx = (target.x) - a.x;
		local dy= (target.y) - a.y;
		if (asDegrees) then 
			return asDegrees(math.atan2(dy, dx));
		else
			return math.atan2(dy, dx);
		end
end

function Utils:random(min,max)
	local randomNum = math.random(((max - min) + 1) + min);
	return randomNum;
end

--- Round towards zero
function Utils:round(value) 
     if value >= 0 then 
     	return math.floor(value+.5) 
      else 
      	return math.ceil(value-.5) end
end

--- Clamps a number between two values
function Utils:bound(value,min,max)
	if value < min then 
		value = min
	end
	if value > max then 
		value = max
	end
	return value
end

--[[
	Find the angle (in radians) between the two Sprites, taking their x/y and origin into account.
	The angle is calculated in clockwise positive direction (down = 90 degrees positive, right = 0 degrees positive, up = 90 degrees negative)
	* @param	a			The Sprites to test from
	* @param	b			The Sprites to test to
	* @param	asDegrees	If you need the value in degrees instead of radians, set to true
	* 
	* @return	Number The angle (in radians unless asDegrees is true)
---]]
function Utils:angleBetween(a,b,asDegrees)
		local dx = (b.x) - (a.x);
		local dy= (b.y) - (a.y);
		
		if asDegrees then 
			return self:asDegrees(math.atan2(dy, dx));
		else
			return math.atan2(dy, dx);
		end
end

--[[
	 * Sets the source Sprites x/y velocity so it will move directly towards the destination Sprites at the speed given (in pixels per second)<br>
	 * The source object doesn't stop moving automatically should it ever reach the destination coordinates.<br>
	 * Note: Doesn't take into account acceleration, maxVelocity or drag (if you set drag or acceleration too high this object may not move at all)
	 * 
	 * @param	source		The Sprites on which the velocity will be set
	 * @param	dest		The Sprites where the source object will move to
	 * @param	speed		The speed it will move, in pixels per second
	 ---]]
function Utils:moveTowardsObject(source,dest,speed)
		local a = self:angleBetween(source, dest,false);
		
		source.xv = math.cos(a) * speed
		source.yv = math.sin(a) * speed
end
