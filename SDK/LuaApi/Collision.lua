--- Author: Murii

-- Common functions

local remove = table.remove

local sqrt = math.sqrt
local cos = math.cos
local atan2 = math.atan2

-- Constants

local pi = math.pi
local pi2 = pi*2

-- Partitioning

local quad = dofile("QuadTree.lua")

local qinsert = quad.insert
local qremove = quad.remove
local qselect = quad.select
local qselectR = quad.selectRange

-- Collisions

local shape = dofile("Shapes.lua")
local getBounds = shape.bounds
print(shape)
-- Internal data

-- list of shapes
local statics = {}
local dynamics = {}
local kinematics = {}

-- global gravity
local gravityx = 0
local gravityy = 0
-- maximum velocity limit of moving shapes
local maxVelocity = 350
-- broad phase partitioning method
local partition = "quad"
-- buffer reused in queries
local buffer = {}
-- some stats
local nchecks = 0

-- Internal functionality

-- returns shape index and its list
local function findShape(s)
   local t = s.list
   for i = 1, #t do
	  if t[i] == s then
		 return i, t
	  end
   end
end

-- repartition moved or modified shapes
local function repartition(s)
   if partition == "quad" then
	  -- reinsert in the quadtree
	  local x, y, hw, hh = shape.bounds(s)
	  qinsert(s, x, y, hw, hh)
   end
end

local function addShapeType(list, t, ...)
   assert(shape.new[t], "invalid shape type")
   local s = shape.new[t](...)
   s.list = list
   list[#list + 1] = s
   repartition(s)
   return s
end

-- changes the position of a shape
local function changePosition(a, dx, dy)
   shape.position(a, dx, dy)
   repartition(a)
end

-- resolves collisions
local function solveCollision(a, b, nx, ny, pen)
   -- shape a must be dynamic
   --assert(a.list == dynamics, "collision pair error")
   local vx, vy = a.xv - (b.xv or 0), a.yv - (b.yv or 0)
   local dp = vx*nx + vy*ny
   -- objects moving towards each other?
   if dp < 0 then
	  -- find separation velocity
	  -- project velocity onto the collision normal
	  local sx, sy = nx*dp, ny*dp
	  -- find tangent velocity
	  local tx, ty = vx - sx, vy - sy
	  -- find collision impulse
	  local r = 1 + a.bounce
	  local f = a.friction
	  local dvx = sx*r + tx*f
	  local dvy = sy*r + ty*f
	  -- adjust the velocity of shape a
	  a.xv = a.xv - dvx
	  a.yv = a.yv - dvy
	  if b.list == dynamics then
		 local arc = atan2(vy, vx) - atan2(ny, nx)
		 arc = (arc + pi)%(pi2) - pi
		 --assert(arc <= pi and arc >= -pi, "collision angle error")
		 local p = cos(arc)
		 -- adjust the velocity of shape b
		 b.xv = b.xv - dvx*p
		 b.yv = b.yv - dvy*p
	  end
   end
   -- find the separation vector
   local sx, sy = nx*pen, ny*pen
   --assert(sx ~= 0 or sy ~= 0, "collision separation error")
   -- store the separation for shape a
   a.sx = a.sx + sx
   a.sy = a.sy + sy
   -- separate the pair by moving shape a
   changePosition(a, sx, sy)
end

-- check and report collisions
local function checkCollision(a, b, dt)
   -- track the number of collision checks (optional)
   nchecks = nchecks + 1
   local nx, ny, pen = shape.test(a, b, dt)
   if pen == nil then
	  return
   end

   --assert(pen > 0, "collision penetration error")
   -- collision callbacks
   local ra = true
   local rb = true
   if a.onCollide then
	  ra = a:onCollide(b, nx, ny, pen)
   end
   if b.onCollide then
	  rb = b:onCollide(a, -nx, -ny, pen)
   end
   -- ignore collision if either callback returned false
   if ra == true and rb == true then
	  solveCollision(a, b, nx, ny, pen)
   end
end

-- Public functionality
collision = {}

-- updates the simulation
function collision.update(dt)
   -- track the number of collision checks (optional)
   nchecks = 0

   -- todo: user defined velocities change
   -- as we add gravity before resolving collisions
   
   -- update velocity vectors
   local xg = gravityx*dt
   local yg = gravityy*dt
   local mv2 = maxVelocity*maxVelocity
   for i = 1, #dynamics do
	  local d = dynamics[i]
	  -- damping
	  local c = 1 + d.damping*dt
	  local xv = d.xv/c
	  local yv = d.yv/c
	  -- gravity
	  local g = d.gravity or 1
	  xv = xv + xg*g
	  yv = yv + yg*g
	  -- threshold
	  local v2 = xv*xv + yv*yv
	  if v2 > mv2 then
		 local n = maxVelocity/sqrt(v2)
		 xv = xv*n
		 yv = yv*n
	  end
	  d.xv = xv
	  d.yv = yv
	  -- reset separation
	  d.sx = 0
	  d.sy = 0
   end
   
   -- move kinematic shapes
   for i = 1, #kinematics do
	  local k = kinematics[i]
	  changePosition(k, k.xv*dt, k.yv*dt)
   end
   -- move dynamic shapes
   if partition == 'quad' then
	  -- quadtree partitioning
	  for i = 1, #dynamics do
		 local d = dynamics[i]
		 -- move to new position
		 changePosition(d, d.xv*dt, d.yv*dt)
		 -- check and resolve collisions
		 -- query for potentially colliding shapes
		 --qselect(d, buffer)
		 local x, y, hw, hh = getBounds(d)
		 qselectR(x, y, hw, hh, buffer)
		 -- note: we check each collision pair twice
		 for j = #buffer, 1, -1 do
			local d2 = buffer[j]
			if d2 ~= d then
			   checkCollision(d, d2, dt)
			end
			-- clear the buffer during iteration
			buffer[j] = nil
		 end
	  end
   end
end

-- gets the global gravity
function collision.getGravity()
   return gravityx, gravityy
end

-- sets the global gravity
function collision.setGravity(x, y)
   gravityx, gravityy = x, y
end

-- static shapes do not move or respond to collisions
function collision.addStatic(shape,tag, ...)
   local s = addShapeType(statics, shape, ...)
   s.tag = tag
   return s
end

-- dynamic shapes are affected by gravity and collisions
function collision.addDynamic(shape,tag, ...)
   local s = addShapeType(dynamics, shape, ...)
   s.friction = 1
   s.bounce = 0
   s.damping = 0
   s.xv, s.yv = 0, 0
   s.sx, s.sy = 0, 0
   s.tag = tag
   return s
end

-- kinematic shapes move only when assigned a velocity
function collision.addKinematic(shape,tag, ...)
   local s = addShapeType(kinematics, shape, ...)
   s.xv, s.yv = 0, 0
   s.tag = tag
   return s
end

-- removes shape from its list
function collision.removeShape(s)
   local i, t = findShape(s)
   if i then
	  s.list = nil
	  remove(t, i)
	  qremove(s)
   end
end

-- gets the position of a shape (starting point for line shapes)
function collision.getPosition(a)
   return a.x, a.y
end

-- sets the position of a shape
function collision.setPosition(a, x, y)
   changePosition(a, x - a.x, y - a.y)
end

-- gets the velocity of a shape
function collision.getVelocity(a)
   return a.xv or 0, a.yv or 0
end

-- sets the velocity of a shape
function collision.setVelocity(a, xv, yv)
   a.xv = xv
   a.yv = yv
end

-- sets the max velocity 
function collision.setMaxVelocity(max)
   maxVelocity = max
end


-- gets the separation of a shape for the last frame
function collision.getDisplacement(a)
   return a.sx or 0, a.sy or 0
end

-- sets the partitioning method
function collision.setPartition(p)
   assert(p == "quad" or p == "none", "invalid partitioning method")
   partition = p
end

-- gets the partitioning method
function collision.getPartition()
   return partition
end

-- gets the number of collision checks for the last frame
function collision.getCollisionCount()
   return nchecks
end

-- Public access to some tables

collision.repartition = repartition
collision.statics = statics
collision.dynamics = dynamics
collision.kinematics = kinematics

return collision
