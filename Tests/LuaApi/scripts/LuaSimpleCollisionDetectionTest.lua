dofile("SDK/Graphics2D.lua")

loadCameraSkybox(getCurrentCamera(), "")

local sprite = Sprite(120,120,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite)

local sprite2 = Sprite(10,120,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite2)


local o1 = collision:addStatic('rect','tag1',120,120,70,70)
local o2 = collision:addDynamic('rect','tag2',-100,120,70,70)
o2.friction = 60
o2.damping = 0.6

setCanvasCameraPosition(mainCanvas.canvas, {320,0})

function getDelta()
   return 1/60
end

function onSceneUpdate()

    --Position of the sprite needs to be the same as the collision body's pos
    sprite2:setPosition(o2.x,o2.y)
    
    if isKeyPressed("RIGHT") then
		o2.xv = o2.xv + 120 * getDelta()
	elseif isKeyPressed("LEFT") then
		o2.xv = o2.xv - 120 * getDelta()
	end
	if isKeyPressed("UP") then
		o2.yv = o2.yv - 120 * getDelta()
	elseif isKeyPressed("DOWN") then
		o2.yv = o2.yv + 120 * getDelta()
	end

	--dont forget to update everything 
    collision:update(getDelta())
end
