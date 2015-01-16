--- Author: Murii

Collision = {}
Collision.__index = Collision

function Collision.create()
	local self = {}
	setmetatable(self,Collision)
	self.shapeCtors = {}
	return self
end

-- local references to common functions
local assert = assert
local insert = table.insert
local remove = table.remove
local ipairs = ipairs

local min = math.min
local max = math.max
local abs = math.abs
local sqrt = math.sqrt
local cos = math.cos
local atan2 = math.atan2

statics = {}
dynamics = {}
kinematics = {}
---DO NOT call this
gravity = 0
---DO NOT call this
maxVelocity = 60

buffer = {}
cchecks = 0
mshapes = 0
partition = "quad"

---set the gravity for the hole world
function Collision:setGravity(value)
  gravity = value
end

function Collision:getGravity()
  return gravity
end

---set the max velocity an object can reach
function Collision:setMaxVelocity(value)
  maxVelocity = value
end
-- remove all elements from a numerically indexed table
function Collision:clearBuffer(b)
  for i = 1, #b do
    b[i] = nil
  end
end

--- clamps number between two values
function Collision:clamp(n, low, high)
  if n < low then
    return low
  elseif n > high then
    return high
  end
  return n
end

--- clamps vector size if above a given length
function Collision:clampVec(x, y, len)
  local d = sqrt(x*x + y*y)
  if d > len then
    local n = 1/d * len
    x, y = x * n, y * n
  end
  return x, y
end

--- returns shape index and its list
function Collision:findShape(s)
  local t = s.list
  for i, v in ipairs(t) do
    if v == s then
      return i, t
    end
  end
end

--- removes shape from its list
function Collision:removeShape(s)
  local i, t = self:findShape(s)
  if i then
    s.list = nil
    remove(t, i)
    quad:remove(s)
  end
end

--- returns center position and half width/height extents for any shape
function Collision:getBounds(s)
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
    hw = (maxx - minx)*.5
    hh = (maxy - miny)*.5
    -- get the midpoint
    x = (maxx + minx)*.5
    y = (maxy + miny)*.5
  end
  return x, y, hw, hh
end
--- returns center position and half width/height extents for any shape
function Collision.getBounds(s)
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
    hw = (maxx - minx)*.5
    hh = (maxy - miny)*.5
    -- get the midpoint
    x = (maxx + minx)*.5
    y = (maxy + miny)*.5
  end
  return x, y, hw, hh
end

--- repartition moved or modified shapes
function Collision.repartition(s)
  if partition == "quad" then
    -- reinsert in the quadtree
    local x, y, hw, hh = Collision.getBounds(s)
    quad:insert(s, x, y, hw, hh)
  end
end

--- tests a point versus any shape
function Collision:testPoint(s, x, y, line)
  line = line or 1
  if s.shape == 'rect' then
    local dx = abs(x - s.x)
    local dy = abs(y - s.y)
    return dx <= s.hw and dy <= s.hh
  elseif s.shape == 'circle' then
    local dx = abs(x - s.x)
    local dy = abs(y - s.y)
    return dx*dx + dy*dy <= s.r*s.r
  elseif s.shape == 'line' then
    local dx, dy = s.x2 - s.x, s.y2 - s.y
    local nx, ny = x - s.x, y - s.y
    local d1 = nx*dx + ny*dy
    local d2 = dx*dx + dy*dy
    --assert(d2 ~= 0, "segment has zero length")
    local u = d1/d2
    if u < 0 then
      u = 0
    else
      if u > 1 then
        u = 1
      end
    end
    local qx = u*dx - nx
    local qy = u*dy - ny
    return qx*qx + qy*qy <= line*line
  end
end

--- test a point versus all shapes
function Collision:queryPoint(result, x, y)
  if partition == 'quad' then
    -- query using the quadtree
    quad:select(result, x, y, 1, 1)
    -- eliminate non-intersecting shapes
    for i = #result, 1, -1 do
      local b = result[i]
      if not self:testPoint(b, x, y) then
        remove(result, i)
      end
    end
  else
    -- brute force query
    for i, s in ipairs(statics) do
      if self:testPoint(s, x, y) then
        insert(result, s)
      end
    end
    for i, s in ipairs(kinematics) do
      if self:testPoint(s, x, y) then
        insert(result, s)
      end
    end
    for i, s in ipairs(dynamics) do
      if self:testPoint(s, x, y) then
        insert(result, s)
      end
    end
  end
end

--- test a shape versus all shapes
function Collision:queryShape(result, a, dt)
  dt = dt or 0
  if partition == 'quad' then
    -- query using the quadtree
    local x, y, hw, hh = self:getBounds(a)
    quad:select(result, x, y, hw, hh)
    -- eliminate non-intersecting shapes
    for i = #result, 1, -1 do
      local b = result[i]
      if a == b or not testShapes(a, b, dt) then
        remove(result, i)
      end
    end
  else
    -- brute force query
    for i, b in ipairs(statics) do
      if a ~= b and testShapes(a, b, dt) then
        insert(result, b)
      end
    end
    for i, b in ipairs(kinematics) do
      if a ~= b and testShapes(a, b, dt) then
        insert(result, b)
      end
    end
    for i, b in ipairs(dynamics) do
      if a ~= b and testShapes(a, b, dt) then
        insert(result, b)
      end
    end
  end
end

--- rects have a center position and half-width/height extents
function Collision.addRectShape(list, x, y, w, h)
  local s = { list = list, shape = "rect", x = x, y = y, hw = w*.5, hh = h*.5}
  insert(list, s)
  Collision.repartition(s)
  return s
end

--- circles have a center position and radius
function Collision.addCircleShape(list, x, y, r)
  local s = { list = list, shape = "circle", x = x, y = y, r = r }
  insert(list, s)
  repartition(s)
  return s
end

--- line shapes have a starting and an ending point
function Collision.addLineShape(list, x, y, x2, y2)
  local s = { list = list, shape = "line", x = x, y = y, x2 = x2, y2 = y2 }
  insert(list, s)
  repartition(s)
  return s
end

function Collision:init()
	self.shapeCtors =
	{
  	rect = Collision.addRectShape, 
  	circle = Collision.addCircleShape, 
  	line = Collision.addLineShape
	}	
end


-- static shapes do not move or respond to collisions
function Collision:addStatic(shape,tag, ...)
  local s = self.shapeCtors[shape](statics, ...)
  s.tag = tag
  return s
end

-- dynamic shapes are affected by gravity and collisions
function Collision:addDynamic(shape,tag, ...)
  local s = self.shapeCtors[shape](dynamics, ...)
  s.friction = 1
  s.bounce = 0
  s.damping = 0
  s.xv, s.yv = 0, 0
  s.tag = tag
  return s
end

-- kinematic shapes move only when assigned a velocity
function Collision:addKinematic(shape,tag, ...)
  local s = self.shapeCtors[shape](kinematics, ...)
  s.xv, s.yv = 0, 0
  s.tag = tag
  return s
end


--- tests two rectangles for intersection
function testRectRect(a, b, dt)
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

--- tests rectangle versus circle
function testRectCircle(a, b, dt)
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

--- tests rectangle versus line segment
local function testRectLine(a, b, dt)
  -- fast bounding box elimination
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
  local dsq = dx*dx + dy*dy
  local d = sqrt(dsq)
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
  assert(v >= 0)
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

--- tests two circles
function testCircleCircle(a, b, dt)
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
  -- todo: dist == 0 when the circles have the same position?
  if dist > 0 then
    -- penetration depth equals the sum of the radii
    -- minus the distance between the intersecting circles
    local pen = radii - dist
    -- collision normal is the normalized vector between the circles
    return dx/dist, dy/dist, pen
  end
end

--- tests circle versus a line segment
function testCircleLine(a, b, dt)
  -- normalize segment
  local dx, dy = b.x2 - b.x, b.y2 - b.y
  local dsq = dx*dx + dy*dy
  local d = sqrt(dsq)
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

-- signed area of a triangle
function Collision:testLineLine(a, b, dt)
  --assert(false, "dynamic line collision unsupported")
end

shapeTests =
{
  rect = { rect = testRectRect, circle = testRectCircle, line = testRectLine },
  circle = { circle = testCircleCircle, line = testCircleLine },
  line = { line = testLineLine }
}

--- returns normalized separation vector and penetration
function Collision:testShapes(a, b, dt)
  -- find collision function
  local test = shapeTests[a.shape][b.shape]
  local r = false
  -- swap the colliding shapes?
  if test == nil then
    test = shapeTests[b.shape][a.shape]
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

--- moves shape by given amount without checking for collisions
function Collision:moveShape(a, dx, dy)
  if a.shape == 'circle' or a.shape == 'rect' then
    a.x = a.x + dx
    a.y = a.y + dy
  elseif a.shape == 'line' then
    a.x = a.x + dx
    a.y = a.y + dy
    a.x2 = a.x2 + dx
    a.y2 = a.y2 + dy
  end
  Collision.repartition(a)
end

--- repositions the shape to given coordinates
function Collision:positionShape(a, x, y)
  if a.shape == 'circle' or a.shape == 'rect' then
    a.x = x
    a.y = y
  elseif a.shape == 'line' then
    local dx, dy = a.x2 - a.x, a.y2 - a.y
    a.x = x
    a.y = y
    a.x2 = x + dx
    a.y2 = y + dy
  end
  Collision.repartition(a)
end

--- returns the velocity of shape
function Collision:getVelocity(a)
  return a.xv, a.yv
end

--- assigns velocity to shape
function Collision:setVelocity(a, xv, yv)
  a.xv = xv
  a.yv = yv
end

--- updates the simulation
function Collision:update()
  -- track some stats
  mshapes = 0
  cchecks = 0

  -- update velocity vectors
  local grav = gravity--*dt
  for i = 1, #dynamics do
    local d = dynamics[i]
    -- damping
    local damp = self:clamp(1 - d.damping, 0, 1)
    d.xv = d.xv*damp
    d.yv = d.yv*damp
    -- gravity
    d.yv = d.yv + grav*(d.gravity or 1)
    d.xv, d.yv = self:clampVec(d.xv, d.yv, maxVelocity)
  end
  
  -- move kinematic shapes
  for i = 1, #kinematics do
    local k = kinematics[i]
    self:moveShape(k, k.xv, k.yv)
    mshapes = mshapes + 1
  end

  -- move dynamic shapes
  if partition == 'quad' then
    -- quadtree
    for i = 1, #dynamics do
      local d = dynamics[i]
      self:moveShape(d, d.xv, d.yv)
      mshapes = mshapes + 1
      -- get area covered by the shape
      local x, y, hw, hh = self:getBounds(d)
      -- reuse buffer so we don't create tables all the time
     -- self:clearBuffer(buffer)
      --quad:select(buffer, x, y, hw, hh)
      for j, s in ipairs(buffer) do
        if s ~= d then
          self:collision(d, s)
          cchecks = cchecks + 1
        end
      end
    end
    -- brute force
    for i = 1, #dynamics do
      local d = dynamics[i]
      self:moveShape(d, d.xv, d.yv)
      mshapes = mshapes + 1
      -- check and resolve collisions
      for j, s in ipairs(statics) do
        cchecks = cchecks + 1
        self:collision(d, s)
      end
      for j, k in ipairs(kinematics) do
        cchecks = cchecks + 1
        self:collision(d, k)
      end
      for j = i + 1, #dynamics do
        cchecks = cchecks + 1
        self:collision(d, dynamics[j])
      end
    end
  end
end

--- check and report collisions
function Collision:collision(a, b)
  local nx, ny, pen = self:testShapes(a, b)
  if pen == nil then
    return
  end
  assert(pen > 0, a.shape .. ' ' .. b.shape)
  -- user resolution
  local ra = true
  local rb = true
  if a.onCollide then
    ra = a:onCollide(b, nx, ny, pen)
  end
  if b.onCollide then
    rb = b:onCollide(a, -nx, -ny, pen)
  end
  if ra == true and rb == true then
    self:solveCollision(a, b, nx, ny, pen)
  end
end

--- resolves collisions
function Collision:solveCollision(a, b, nx, ny, pen)
  -- shape a must be dynamic
  assert(a.list == dynamics)
  
  local vx, vy = a.xv - (b.xv or 0), a.yv - (b.yv or 0)
  local dp = vx*nx + vy*ny
  -- objects moving towards each other?
  if dp < 0 then
    -- project velocity onto collision normal
    local pnx, pny = nx*dp, ny*dp
    -- find tangent velocity
    local tx, ty = vx - pnx, vy - pny
    -- respond to the collision
    local r = 1 + a.bounce
    local f = a.friction
    local dvx = pnx*r + tx*f
    local dvy = pny*r + ty*f
    a.xv = a.xv - dvx
    a.yv = a.yv - dvy

    -- apply force to shape b
    if b.list == dynamics then
      local ar = atan2(vy, vx) - atan2(ny, nx)
      local force = cos(ar)
      b.xv = b.xv - dvx*force
      b.yv = b.yv - dvy*force
    end
  end
  -- separate
  if abs(nx) > 0 or abs(ny) > 0 then
    self:moveShape(a, nx*pen, ny*pen)
  end
end
