--- Author: Murii

-- references to common functions
local tremove = table.remove

-- quadrant names
local rdirs = { nw='se', sw='ne', ne='sw', se='nw' }

-- minimum cell size
local mincellsize = 8

-- some stats (optional)
local nobjects = 0
local nlivecells = 0
local ndeadcells = 0

-- internals
local root = nil
local handles = {}
local deadcells = {}

-- create cell
local function createC(p, q, x, y, s)
   -- get a cell from the pool
   local c = tremove(deadcells) or {}
   -- parent cell
   c.parent = p
   -- quadrant in parent cell
   c.quad = q
   -- position
   c.x = x
   c.y = y
   -- half-width/height extent
   c.s = s
   c.nchildren = 0
   -- update parent
   if p then
	  p.nchildren = p.nchildren + 1
	  p[q] = c
   end
   -- update the stats
   nlivecells = nlivecells + 1
   ndeadcells = #deadcells
   return c
end

-- destroy cell
local function destroyC(c)
   -- delete reference from parent to child
   local p = c.parent
   if p then
	  p[c.quad] = nil
	  p.nchildren = p.nchildren - 1
   end
   -- clear references
   c.nw = nil
   c.ne = nil
   c.sw = nil
   c.se = nil
   deadcells[#deadcells + 1] = c
   -- update the stats
   nlivecells = nlivecells - 1
   ndeadcells = #deadcells
end

-- create a new root cell
local function createR(x, y, s)
   if x < 0 then
	  x = -x
   end
   if y < 0 then
	  y = -y
   end
   local d = x
   if d < y then
	  d = y
   end
   d = d + s
   d = d*2
   local n = math.ceil(math.log(d)/math.log(mincellsize))
   local rs = math.pow(mincellsize, n)
   assert(rs >= mincellsize)
   root = createC(nil, nil, 0, 0, rs)
   return root
end

-- returns quad direction and offset
local function getQuad(c, x, y)
   if x < c.x then
	  if y < c.y then
		 return 'nw', -1, -1
	  else
		 return 'sw', -1, 1
	  end
   else
	  if y < c.y then
		 return 'ne', 1, -1
	  else
		 return 'se', 1, 1
	  end
   end
end

-- returns true if the object fits inside a cell
local function fitsInCell(c, x, y, s)
   local dx = c.x - x
   local dy = c.y - y
   if dx < 0 then
	  dx = -dx
   end
   if dy < 0 then
	  dy = -dy
   end
   local d = dx
   if dy > d then
	  d = dy
   end
   return d + s < c.s/2
   --local cs4 = c.s/4
   --return s < cs4 and dx < cs4 and dy < cs4
end

local function expandUp(x, y, s)
   if fitsInCell(root, x, y, s) then
	  return root
   end
   -- expand tree upwards
   local d, ox, oy = getQuad(root, x, y)
   d = rdirs[d]
   local q = root.s/2
   ox = ox*q + root.x
   oy = oy*q + root.y
   -- create new root
   local c = createC(nil, nil, ox, oy, root.s*2)
   c.nchildren = 1
   c[d] = root
   root.quad = d
   root.parent = c
   -- assign new root
   root = c
   expandUp(x, y, s)
end

-- returns cell which fits an object of given size
-- creates the cell if necessary
local function expandDown(c, x, y, s)
   local cs = c.s
   local cs4 = cs/4
   -- can't fit in a child cell?
   if s >= cs4 or cs == mincellsize then
	  return c
   end
   --assert(fitsInCell(c, x, y, s), "object is larger than cell")
   -- find which sub-cell the object belongs to
   local d, ox, oy = getQuad(c, x, y)
   -- create sub-cell if necessary
   if c[d] == nil then
	  ox = ox*cs4 + c.x
	  oy = oy*cs4 + c.y
	  createC(c, d, ox, oy, cs/2)
   end
   -- descend deeper down the tree
   return expandDown(c[d], x, y, s)
end

-- trim from the bottom up
-- removing empty cells
local function trimBottom(c)
   if #c > 0 or c.nchildren > 0 then
	  return
   end
   local p = c.parent
   destroyC(c)
   if c == root then
	  root = nil
	  return
   end
   trimBottom(p)
end

-- trim from the top down
-- removing empty cells or cells that have only have one child
local function trimTop()
   if root == nil or #root == 0 then
	  return
   end
   -- root has one child only?
   if root.nchildren ~= 1 then
	  return
   end
   -- get the only child node
   local c = root.nw or root.ne or root.sw or root.se
   -- severe the link between child and parent
   local nroot = c
   nroot.quad = nil
   nroot.parent = nil
   -- before we remove the old root
   -- make sure it doesn't point to its only child
   root[c] = nil
   destroyC(root)
   -- assign new root
   root = nroot
   trimTop()
end

-- remove object
local function removeO(object)
   local c = handles[object]
   -- removing a non-existing object?
   if c == nil then
	  return
   end
   nobjects = nobjects - 1
   handles[object] = nil
   -- todo: make constant time
   for i = 1, #c do
	  if c[i] == object then
		 tremove(c, i)
		 break
	  end
   end
   trimBottom(c)
end

-- insert new object
local function insertO(object, x, y, hw, hh)
   local s = hw
   if s < hh then
	  s = hh
   end

   -- remove object from current cell
   local c = handles[object]
   if c then
	  -- object remains in its current cell?
	  if fitsInCell(c, x, y, s) then
		 --return
	  end
	  removeO(object)
   end

   -- add root cell
   if root == nil then
	  createR(x, y, s)
	  local sz = root.x .. "," .. root.y .. "x" .. root.s
	  sz = sz .. "=>" .. x .. "," .. y .. "x" .. s
	  assert(fitsInCell(root, x,y,s), sz)
   end

   -- expand tree up
   expandUp(x, y, s)
   
   local c2 = root
   -- expand tree down
   c2 = expandDown(root, x, y, s)

   -- insert object
   c2[#c2 + 1] = object
   handles[object] = c2
   nobjects = nobjects + 1
end

-- select entire cell
local function selectC(c, dest)
   -- insert all objects in this cell
   local n = #dest
   for i = 1, #c do
	  dest[n + i] = c[i]
   end
   -- descent down the tree
   if c.nw then
	  selectC(c.nw, dest)
   end
   if c.ne then
	  selectC(c.ne, dest)
   end
   if c.se then
	  selectC(c.se, dest)
   end
   if c.sw then
	  selectC(c.sw, dest)
   end
end

-- select part of a cell
local function selectCR(c, x, y, hw, hh, dest)
   local dx = c.x - x
   local dy = c.y - y
   if dx < 0 then
	  dx = -dx
   end
   if dy < 0 then
	  dy = -dy
   end
   -- range is outside of the cell
   local s = c.s
   if s + hw < dx or s + hh < dy then
	  return
   end
   -- range covers the cell entirely
   if s + dx < hw and s + dy < hh then
	  return selectC(c, dest)
   end
   -- range covers the cell partially
   -- insert all objects in this cell
   local n = #dest
   for i = 1, #c do
	  dest[n + i] = c[i]
   end
   -- descent down the tree
   if c.nw then
	  selectCR(c.nw, x, y, hw, hh, dest)
   end
   if c.ne then
	  selectCR(c.ne, x, y, hw, hh, dest)
   end
   if c.se then
	  selectCR(c.se, x, y, hw, hh, dest)
   end
   if c.sw then
	  selectCR(c.sw, x, y, hw, hh, dest)
   end
   return dest
end

-- select by given range
local function selectR(x, y, w, h, dest)
   dest = dest or {}
   -- tree is empty?
   if root == nil then
	  return dest
   end
   return selectCR(root, x, y, w, h, dest)
end

-- select by object
local function selectO(object, dest)
   dest = dest or {}
   -- get the cell of the object
   local c = handles[object]
   -- object not in the tree?
   if c == nil then
	  return dest
   end
   return selectR(c.x, c.y, c.s, c.s, dest)
end

-- get the cell range of an object
local function getR(object)
   local c = handles[object]
   -- object not in the tree?
   if c == nil then
	  return dest
   end
   return c.x, c.y, c.s, c.s
end

quad = {}

quad.insert = insertO
quad.remove = removeO
quad.select = selectO
quad.selectRange = selectR
quad.trimTop = trimT
quad.getRange = getR

return quad
