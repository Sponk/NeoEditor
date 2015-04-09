dofile("SDK/Graphics2D.lua")
print("Benchmark")
local batch = SpriteBatch()
local file = "maps/neo-icon.png"
local numSprites = 12000
local maxScale = 1
local res = getWindowScale()

math.randomseed(os.time())

for i = 1, numSprites, 1 do
   local s = batch:createSprite(file)
   s:setPosition(math.random(0, res[1]), math.random(0, res[2]))
   s.xdir = 1 - (math.random() * 2)
   s.ydir = 1 - (math.random() * 2)

   s.rotSpeed = math.random(-200, 200)
   s.speed = math.random(5, 50)
   s.scaleSpeed = math.random(-100, 100) * 0.001
   s.scales = 0

   s:update()
end

mainCanvas:addBatch(batch)

function onSceneUpdate(delta)
   for i,s in pairs(batch.sprites) do
	  s:translate(s.xdir*s.speed, s.ydir*s.speed * delta)
	  s:rotate(s.rotSpeed * delta)

	  s.scales = s.scales + s.scaleSpeed * delta
	  s:setScale(s.scales, s.scales)

	  s:update()

	  if s.scales >= maxScale then s.scaleSpeed = -s.scaleSpeed end
	  if s.position[1] >= res[1] or s.position[1] < 0 then s.xdir = -s.xdir end
	  if s.position[2] >= res[2] or s.position[2] < 0 then s.ydir = -s.ydir end
   end
end
