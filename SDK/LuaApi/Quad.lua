--- Author: Murii

Quad = {}
Quad.__index = Quad

function Quad.create()
	local self = {}
	setmetatable(self, Quad)

	-- local references to common functions
	self.table = table
	self.ipairs = ipairs
	self.abs = math.abs
	self.max = math.max
	self.tinsert = table.insert
	self.tremove = table.remove
	-- quadrant names
	self.dirs = { 'nw', 'sw', 'ne', 'se' }
	self.rdirs = { nw='se', sw='ne', ne='sw', se='nw' }
	-- tree structure
	self.root = nil
	self.handles = {}
	self.deadcells = {}
	self.mincellsize = 8

	-- some stats
	self.objects = 0
	self.livecells = 0

	return self
end
--- Creates new cell
function Quad:createCell(p, r, x, y, s)
  local c
  -- are there any unused cells in the pool?
  if #self.deadcells == 0 then
    c = {}
  else
    c = self.tremove(self.deadcells)
    c.nw = nil
    c.ne = nil
    c.sw = nil
    c.se = nil
  end
  c.parent = p
  c.relation = r
  c.x = x
  c.y = y
  c.side = s
  -- update the livecells count
  self.livecells = self.livecells + 1
  return c
end

--- Destroys existing cell
function Quad:destroyCell(c)
  -- delete reference from parent to child
  if c.parent then
    c.parent[c.relation] = nil
  end
  -- update the livecells count
  self.livecells = self.livecells - 1
  self.tinsert(self.deadcells, c)
  -- is this the root cell?
  if c == root then
    root = nil
  end
end

--- Returns number of child cells
function Quad:getChildCount(c)
  local n = 0
  for _, v in self.ipairs(self.dirs) do
    if c[v] then
      n = n + 1
    end
  end
  return n
end

--- Returns true if cell has child cells
function Quad:hasChildren(c)
  return c.nw ~= nil or c.sw ~= nil or c.ne ~= nil or c.se ~= nil
end

--- Returns true if object fits entirely inside cell
function Quad:fitsInCell(c, x, y, s)
  local dx = self.abs(c.x - x)
  local dy = self.abs(c.y - y)
  local e = c.side/4
  return s < e and dx < e and dy < e
end

--- Returns child cell direction and offset
function Quad:getDirection(c, x, y)
  local d, ox, oy
  if x < c.x then
    if y < c.y then
      d, ox, oy = 'nw', -1, -1
    else
      d, ox, oy = 'sw', -1, 1
    end
  else
    if y < c.y then
      d, ox, oy = 'ne', 1, -1
    else
      d, ox, oy = 'se', 1, 1
    end
  end
  return d, ox, oy
end

--- Returns cell which fits an object of given size
-- creates the cell if necessary
function Quad:getCell(c, x, y, s)
  -- object fits inside this cell?
  local q = c.side/2
  if s*2 > q or q < self.mincellsize then
    return c
  end
  -- find which sub-cell the object belongs to
  local d, ox, oy = self:getDirection(c, x, y)
  -- create sub-cell if necessary
  if c[d] == nil then
    local e = q/2
    ox = ox*e + c.x
    oy = oy*e + c.y
    c[d] = self:createCell(c, d, ox, oy, q)
  end
  -- descend deeper down the tree
  return self:getCell(c[d], x, y, s)
end

--- Select all objects in a cell
function Quad:selectCellAll(root, dest)
  -- insert all objects in this cell
  for i, v in self.ipairs(root) do
    self.tinsert(dest, v)
  end
  for i, v in self.ipairs(self.dirs) do
    local c = root[v]
    if c then
      self:selectCellAll(c, dest)
    end
  end
end

--- Select objects in range
function Quad:selectCell(root, dest, x, y, hw, hh)
  -- insert all objects in this cell
  for _, v in self.ipairs(root) do
    self.tinsert(dest, v)
  end
  -- descent down the tree
  for _, v in self.ipairs(self.dirs) do
    local c = root[v]
    if c then
      local r = c.side
      local dx = math.abs(c.x - x)
      local dy = self.abs(c.y - y)
      -- query intersect this cell?
      if r > dx - hw and r > dy - hh then
        -- query covers the cell entirely?
        if r < hw - dx and r < hh - dy then
          self:selectCellAll(c, dest)
        else
          self:selectCell(c, dest, x, y, hw, hh)
        end
      end
    end
  end
end

--- Trim empty cells from bottom up
function Quad:trimBottom(c)
  while c and #c == 0 and not self:hasChildren(c) do
    local p = c.parent
    self:destroyCell(c)
    c = p
  end
end


--- Trim the top of the quadtree deleting
-- root nodes if they only have a single child
function Quad:trimTop()
  while self.root and #self.root == 0 do
    -- root has one child only?
    local children = self:getChildCount(self.root)
    if children > 1 then
      return
    end
    -- get the only child node
    local child = self.root.nw or self.root.ne or self.root.sw or self.root.se
    if child == nil then
      return
    end
    -- severe the link between child and parent
    local nroot = self.root[child]
    nroot.relation = 'none'
    nroot.parent = nil
    -- before we remove the old root
    -- make sure it doesn't point to its only child
    self.root[child] = nil
    self:destroyCell(self.root)
    -- assign new root
    self.root = nroot
  end
end


--- Insert new object
function Quad:insert(object, x, y, hw, hh)
  local s = math.max(hw, hh)
  --assert(s > 0)
  -- remove object from current cell
  local c = self.handles[object]
  if c then
    -- object remains in its current cell?
    if self:fitsInCell(c, x, y, s) then
      return
    end
    self:remove(object)
  end
  -- add root cell
  if self.root == nil then
    self.root = self:createCell(nil, 'none', 0, 0, s*4)
  end
  -- expand tree if necessary
  while true do
    -- can the object fit in the root cell?
    if self:fitsInCell(self.root, x, y, s) then
      local c = self:getCell(self.root, x, y, s)
      -- insert object
      self.tinsert(c, object)
      self.handles[object] = c
      self.objects = self.objects + 1
      return
    end
    -- expand tree upwards
    local d, ox, oy = self:getDirection(self.root, x, y)
    d = self.rdirs[d]
    local q = self.root.side*.5
    ox = ox*q + self.root.x
    oy = oy*q + self.root.y
    -- create new root
    local nroot = self:createCell(nil, 'none', ox, oy, self.root.side*2)
    nroot[d] = self.root
    self.root.relation = d
    self.root.parent = nroot
    -- assign new root
    self.root = nroot
  end
end

--- Remove object
function Quad:remove(object)
  local c = self.handles[object]
  -- removing a non-existing object?
  if c == nil then
    return
  end
  self.objects = self.objects - 1
  self.handles[object] = nil
  -- todo: make constant time
  for i, v in self.ipairs(c) do
    if v == object then
      self.tremove(c, i)
      break
    end
  end
  self:trimBottom(c)
end

--- Select objects in range
function Quad:select(dest, x, y, hw, hh)
  if self.root then
    self:selectCell(self.root, dest, x, y, hw, hh);
  end
end

--- Select objects in range
function Quad:selectAABB(dest, l, t, r, b)
  -- realign aabb if necessary
  if l > r then
    l, r = r, l
  end
  if t > b then
    t, b = b, t
  end
  local x, y = (l + r)/2, (t + b)/2
  local hw = (r - l)/2
  local hh = (b - t)/2
  self:select(dest, x, y, hw, hh)
end

--- Selects all objects
function Quad:selectAll(dest, x, y, hw, hh)
  if self.root then
    self:selectCellAll(self.root, dest)
  end
end
