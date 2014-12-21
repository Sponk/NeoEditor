dofile("SDK/Graphics2D.lua")
dofile("luaunit.lua")

loadCameraSkybox(getCurrentCamera(), "")

local sprite = Sprite(0,0,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite)

local spriteSheet = SpriteSheet("maps/p1_spritesheet.png", 72, 97, 1)
local tile = Tile(100,100,72,97,spriteSheet,0,0)

mainCanvas:addWidget(tile)

local walkAnimation = SpriteAnimation(0,3,70,true)
local idleAnimation = SpriteAnimation(0,0,70,false)

function onSceneUpdate()
    sprite:rotate(2)

    if isKeyPressed("RIGHT") then
	tile:attachAnimation(walkAnimation)
	tile:translate(2,0)
    elseif isKeyPressed("LEFT") then
	tile:attachAnimation(walkAnimation)
	tile:translate(-2,0)
    else
	tile:attachAnimation(idleAnimation)
    end

    tile:update()
end
