dofile("SDK/Graphics2D.lua")

loadCameraSkybox(getCurrentCamera(), "")

local sprite = Sprite(120,120,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite)

local sprite2 = Sprite(10,120,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite2)


local o1 = Collision.addStatic("rect",'tag1',120,120,70/2,70/2)
local o2 = Collision.addDynamic("rect",'tag2',10,120,70/2,70/2)
o2.friction = 0.5
o2.damping = 1.6

local speed = 1500

setCanvasCameraPosition(mainCanvas.canvas, {320,0})

function onSceneUpdate(delta)

    --Position of the sprite needs to be the same as the collision body's pos
    sprite2:setPosition(o2.x,o2.y)
    
    if isKeyPressed("RIGHT") then
		o2.xv = o2.xv + speed * delta
	elseif isKeyPressed("LEFT") then
		o2.xv = o2.xv - speed * delta
	end
	if isKeyPressed("UP") then
		o2.yv = o2.yv - speed * delta
	elseif isKeyPressed("DOWN") then
		o2.yv = o2.yv + speed * delta
	end

	--dont forget to update everything 
    Collision.update(delta)
end
