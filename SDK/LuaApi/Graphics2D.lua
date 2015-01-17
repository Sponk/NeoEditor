--- The Graphics2D Interface
-- The Graphics2D interface allows you to easily take advantage of
-- the 2D rendering capabilities Neo has. You can easily include
-- this file (Graphics2D.lua) and use all classes and functions
-- related to 2D graphics access.
-- 
-- This interface allows you to easily create in-game UIs, display and manipulate
-- sprites and load and manage sprite sheets and sprite sheet based animations.
-- 
-- Example for loading an animation:
--[[

dofile("SDK/Graphics2D.lua")

local spriteSheet = SpriteSheet("maps/p1_spritesheet.png", 72, 97, 1)
local tile = Tile(100,100,72,97,spriteSheet,0,0)

mainCanvas:addWidget(tile)

local walkAnimation = SpriteAnimation(0,3,70,true)
tile:attachAnimation(walkAnimation)

function onSceneUpdate()
  tile:update()
end

]]
-- 

dofile("class.lua")
dofile("Canvas.lua")
dofile("Sprite.lua")
dofile("Tile.lua")
dofile("SpriteSheet.lua")
dofile("SpriteAnimation.lua")
dofile("Button.lua")
dofile("InputField.lua")
dofile("Label.lua")
dofile("Utils.lua")
dofile("Quad.lua")
dofile("Collision.lua")
dofile("Object3d.lua")

--global instance for Utils class
util = Utils.create()

--collision memebers
quad = Quad.create()
collision = Collision.create()
collision:init()

enableGui(1)
