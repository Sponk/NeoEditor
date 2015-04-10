--- Author: Murii
-- Common functions

local abs = math.abs
local sqrt = math.sqrt

shape = {}

-- Constructors

shape.new = {}

-- rects have a center position and half-width/height extents
function shape.new.rect(x, y, hw, hh)
   return { shape = "rect", x = x, y = y, hw = hw, hh = hh }
end

-- circles have a center position and radius
function shape.new.circle(x, y, r)
   return { shape = "circle", x = x, y = y, r = r }
end

-- line shapes have a starting and an ending point
function shape.new.line(x, y, x2, y2)
   return { shape = "line", x = x, y = y, x2 = x2, y2 = y2 }
end

-- Tests
shape.tests = {}
shape.tests.rect = {}

-- tests two rectangles
function shape.tests.rect.rect(a, b, dt)
   -- vector between the centers of the rects
   local dx, dy = a.x - b.x, a.y - b.y
   -- absolute distance between the centers of the rects
   local adx, ady = abs(dx), abs(dy)
   -- sum of the half-width extents
   local shw, shh = a.hw + b.hw, a.hh + b.hh
   -- no intersection if the sum of the half-width extents
   -- is greater than the distance between the rects
   if adx >= shw or ady >= shh then
	  return
   end
   -- shortest separation for both the x and y axis
   local sx, sy = shw - adx, shh - ady
   if dx < 0 then
	  sx = -sx
   end
   if dy < 0 then
	  sy = -sy
   end
   -- ignore separation for explicitly defined edges
   if sx > 0 then
	  if a.left or b.right then
		 sx = 0
	  end
   elseif sx < 0 then
	  if a.right or b.left then
		 sx = 0
	  end
   end
   if sy > 0 then
	  if a.bottom or b.top then
		 sy = 0
	  end
   elseif sy < 0 then
	  if a.top or b.bottom then
		 sy = 0
	  end
   end
   -- ignore the longer separation axis
   -- when both sx and sy are non-zero
   if abs(sx) < abs(sy) then
	  if sx ~= 0 then
		 sy = 0
	  end
   else
	  if sy ~= 0 then
		 sx = 0
	  end
   end
   -- penetration depth equals
   -- the length of the separation vector
   local pen = sqrt(sx*sx + sy*sy)
   -- todo: dist == 0 when the two rects have the same position?
   if pen > 0 then
	  -- collision normal is the normalized separation vector (sx,sy)
	  return sx/pen, sy/pen, pen
   end
end

-- tests rectangle versus circle
function shape.tests.rect.circle(a, b, dt)
   -- vector between the centers of the two shapes
   local dx, dy = a.x - b.x, a.y - b.y
   -- absolute distance between the centers of the two shapes
   local adx, ady = abs(dx), abs(dy)
   -- find the shortest separation and the penetration depth
   local sx, sy = 0, 0
   local pen = 0
   if adx <= a.hw or ady <= a.hh then
	  -- rectangle edge collision
	  -- check the x and y axis
	  -- no intersection if the distance between the shapes
	  -- is greater than the sum of the half-width extents and the radius
	  if adx >= a.hw + b.r or ady >= a.hh + b.r then
		 return
	  end
	  -- shortest separation vector
	  sx = a.hw + b.r - adx
	  sy = a.hh + b.r - ady
	  -- ignore the longer separation axis
	  -- when both sx and sy are non-zero
	  if sx < sy then
		 if sx ~= 0 then
			sy = 0
		 end
	  else
		 if sy ~= 0 then
			sx = 0
		 end
	  end
	  -- penetration depth
	  pen = sqrt(sx*sx + sy*sy)
   else
	  -- rectangle corner collision
	  -- check the dx and dy axis
	  -- find the nearest point on the rect to the circle center
	  local px, py = 0, 0
	  if adx > a.hw then
		 px = adx - a.hw
	  end
	  if ady > a.hh then
		 py = ady - a.hh
	  end
	  -- no intersection if point is outside of the circle
	  local dist = sqrt(px*px + py*py)
	  if dist >= b.r then
		 return
	  end
	  -- penetration depth equals the circle radius
	  -- minus the distance of the nearest point vector
	  pen = b.r - dist
	  -- shortest separation vector
	  sx, sy = px/dist*pen, py/dist*pen
   end
   -- correct the signs of the separation vector
   if dx < 0 then
	  sx = -sx
   end
   if dy < 0 then
	  sy = -sy
   end
   return sx/pen, sy/pen, pen
end

-- tests rectangle versus line segment
function shape.tests.rect.line(a, b, dt)
   -- quick bounding box elimination
   local minx, maxx = b.x, b.x2
   local miny, maxy = b.y, b.y2
   if b.x > b.x2 then
	  minx, maxx = maxx, minx
   end
   if b.y > b.y2 then
	  miny, maxy = maxy, miny
   end
   -- test the bounding areas of the rect and the line
   if a.x + a.hw < minx or a.x - a.hw > maxx then
	  return
   end
   if a.y + a.hh < miny or a.y - a.hh > maxy then
	  return
   end
   -- normalize segment
   local dx, dy = b.x2 - b.x, b.y2 - b.y
   local d = sqrt(dx*dx + dy*dy)
   -- segment is degenerate
   if d == 0 then
	  return
   end
   local ndx, ndy = dx/d, dy/d
   -- test along the segment axis
   -- project segment origin point
   local s1 = ndx*b.x + ndy*b.y
   -- project segment end point
   local s2 = ndx*b.x2 + ndy*b.y2
   -- project rect center
   local c2 = ndx*a.x + ndy*a.y
   -- project rect extents
   local e = abs(ndx*a.hw) - abs(ndy*a.hh)
   -- adjust the ends of the projected segment (to be revised)
   if b.x == b.x2 then
	  s1 = s1 + e
	  s2 = s2 - e
   elseif b.y == b.y2 then
	  s1 = s1 - e
	  s2 = s2 + e
   elseif (b.x < b.x2) == (b.y < b.y2) then
	  s1 = s1 - e
	  s2 = s2 - e
   else
	  s1 = s1 + e
	  s2 = s2 + e
   end
   -- look for separation
   if c2 < s1 or c2 > s2 then
	  return
   end
   -- test along the normal axis
   -- rotate the segment axis 90 degrees counter-clockwise
   local nx, ny = -ndy, ndx
   -- project velocity
   local xv, yv = a.xv or 0, a.yv or 0
   local v = -(nx*xv + ny*yv)
   -- ignore collision for one-sided segments
   if v <= 0 then
	  return
   end
   -- project segment origin point
   local o = nx*b.x2 + ny*b.y2
   -- project rect center
   local c = nx*a.x + ny*a.y
   -- project rect extents
   local h = abs(nx*a.hw) + abs(ny*a.hh)
   -- find the penetration depth
   local pen = -(c - h - o)
   -- entirely on one side of the segment?
   if pen <= 0 or pen > h*2 then
	  return
   end
   -- was it previously on one side of the segment?
   if v*dt > 0 and pen - v*dt > 1 then
	  return
   end
   return nx, ny, pen
end

shape.tests.circle = {}

-- tests two circles
function shape.tests.circle.circle(a, b, dt)
   -- vector between the centers of the circles
   local dx, dy = a.x - b.x, a.y - b.y
   -- squared distance between the centers of the circles
   local distSq = dx*dx + dy*dy
   -- sum of the radii
   local radii = a.r + b.r
   -- no intersection if the distance between the circles
   -- is greater than the sum of the radii
   if distSq >= radii*radii then
	  return
   end
   -- distance between the centers of the circles
   local dist = sqrt(distSq)
   -- distance is zero when the two circles have the same position
   local nx, ny = 0, 1
   if dist > 0 then
	  nx, ny = dx/dist, dy/dist
   end
   -- penetration depth equals the sum of the radii
   -- minus the distance between the intersecting circles
   local pen = radii - dist
   -- collision normal is the normalized vector between the circles
   return nx, ny, pen
end

-- tests circle versus line segment
function shape.tests.circle.line(a, b, dt)
   -- normalize segment
   local dx, dy = b.x2 - b.x, b.y2 - b.y
   local d = sqrt(dx*dx + dy*dy)
   -- segment is degenerate
   if d == 0 then
	  return
   end
   local ndx, ndy = dx/d, dy/d
   -- test along the segment axis
   local s1 = ndx*b.x + ndy*b.y
   local s2 = ndx*b.x2 + ndy*b.y2
   local c2 = ndx*a.x + ndy*a.y
   if c2 < s1 or c2 > s2 then
	  return
   end
   -- test along the normal axis
   -- rotate the segment axis 90 degrees counter-clockwise
   local nx, ny = -ndy, ndx
   -- project velocity
   local xv, yv = a.xv or 0, a.yv or 0
   local v = -(nx*xv + ny*yv)--*dt
   -- ignore collision for one-sided segments
   if v <= 0 then
	  return
   end
   -- project segment origin
   local o = nx*b.x + ny*b.y
   -- project circle center
   local c = nx*a.x + ny*a.y
   -- find separation
   local pen = -(c - a.r - o)
   -- entirely on one side of the segment?
   if pen <= 0 or pen > a.r*2 then
	  return
   end
   -- was it previously on one side of the segment?
   if v*dt > 0 and pen - v*dt > 1 then
	  return
   end
   return nx, ny, pen
end

shape.tests.line = {}

-- tests two line segments
function shape.tests.line.line(a, b, dt)
   -- assert(false, "dynamic line collision unsupported")
end

-- tests any two shapes
-- returns normalized separation vector and penetration
function shape.test(a, b, dt)
   -- find collision function
   local test = shape.tests[a.shape][b.shape]
   local r = false
   -- swap the colliding shapes?
   if test == nil then
	  test = shape.tests[b.shape][a.shape]
	  a, b = b, a
	  r = true
   end
   local x, y, p = test(a, b, dt)
   -- reverse direction of the collision normal
   if r == true and x and y then
	  x, y = -x, -y
   end
   return x, y, p
end

-- returns center position and half width/height extents for any shape
function shape.bounds(s)
   local x, y = s.x, s.y
   local hw, hh
   local t = s.shape
   if t == "rect" then
	  hw, hh = s.hw, s.hh
   elseif t == "circle" then
	  hw, hh = s.r, s.r
   elseif t == "line" then
	  -- figure out extents
	  local minx, maxx = s.x, s.x2
	  local miny, maxy = s.y, s.y2
	  if minx > maxx then
		 minx, maxx = maxx, minx
	  end
	  if miny > maxy then
		 miny, maxy = maxy, miny
	  end
	  hw = (maxx - minx)/2
	  hh = (maxy - miny)/2
	  -- get the midpoint
	  x = minx + hw
	  y = miny + hh
   end
   return x, y, hw, hh
end

-- changes the position of a shape
function shape.position(a, dx, dy)
   a.x = a.x + dx
   a.y = a.y + dy
   if a.shape == 'line' then
	  a.x2 = a.x2 + dx
	  a.y2 = a.y2 + dy
   end
end

return shape
