--- Utilities
-- This file contains various utility functions.

Utils = {}

Utils.PI = 3.1415927;

Utils.RADTODEG = (180 / self.PI);
Utils.DEGTORAD = (self.PI / 180);

--- Calculates the length of the given vector
-- @param dx The X component
-- @param dy The Y component
-- @return The length as a number.
function Utils.vectorLength(dx,dy)
	return math.sqrt(dx * dx + dy * dy);
end

--- Calculates the distance between two vec2's
--
-- FIXME: Should use plain arrays instead of "a.x" etc.
--
-- @param a The first vector
-- @param b The second vector
-- @return The distance as a number.
function Utils.distanceBetween(a,b)
   local dx = a[1] - b[1];
   local dy =  a[2] - b[2];
   return (self:vectorLength(dx, dy));
end

--- Calculates the angle (in radians) between an two vectors.
-- The angle is calculated in clockwise positive direction (down = 90 degrees positive, right = 0 degrees positive, up = 90 degrees negative)
-- @param a The first vector as vec2
-- @param b The second vector as vec2
-- @param asDegrees A boolean specifying the return format (degrees vs. radians)
-- @return The angle in radians or degrees
function Utils.angleBetween(a,b,asDegrees)
   local dx = (b[1]) - a[1];
   local dy= (b[2]) - a[2];

   if (asDegrees) then 
	  return Utils.RADTODEG * math.atan2(dy, dx);
   else
	  return math.atan2(dy, dx);
   end
end

--- Round a number towards zero
-- @param value A number
-- @return The rounded number as an integer
function Utils.round(value) 
   if value >= 0 then 
	  return math.floor(value + 0.5) 
   else 
	  return math.ceil(value - 0.5) end
end

--- Clamps a number between the given limits
-- @param value The value
-- @param min The minimal value
-- @param max The maximal value
function Utils.clamp(value,min,max)
	if value < min then 
		value = min
	end
	if value > max then 
		value = max
	end
	return value
end
