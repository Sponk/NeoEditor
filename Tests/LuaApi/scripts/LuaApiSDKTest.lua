dofile("SDK/Graphics2D.lua")
dofile("luaunit.lua")

loadCameraSkybox(getCurrentCamera(), "")

local sprite = Sprite(0,0,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite)

local spriteSheet = SpriteSheet("maps/p1_spritesheet.png", 72, 97, 1)
local tile = Tile(100,100,72,97,spriteSheet,0,0)

mainCanvas:addWidget(tile)

local walkAnimation = SpriteAnimation(0,3,150,true)

tile:attachAnimation(walkAnimation)

function onSceneUpdate()
    sprite:rotate(2)
    tile:update()
end
