dofile("SDK/Graphics2D.lua")
dofile("SDK/Neo3D.lua")
dofile("luaunit.lua")

loadCameraSkybox(getCurrentCamera(), "")

local sprite = Sprite(0,0,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite)

local spriteSheet = SpriteSheet("maps/p1_spritesheet.png", 72, 97, 1)
local tile = Tile(100,100,72,97,spriteSheet,0,0)

mainCanvas:addWidget(tile)

local walkAnimation = SpriteAnimation(0,3,70,true)
local idleAnimation = SpriteAnimation(0,0,70,false)

function btnCallback()
    print("Button!")
end

local button = Button(20,500,150,25,"Hello World","btnCallback")
mainCanvas:addWidget(button)

local input = InputField(20, 550, 150, 25, "Hello World")
mainCanvas:addWidget(input)

local label = Label(20, 600, 150, 25, "This is nothing")
mainCanvas:addWidget(label)

label:setLabel("This is a label!")

cube = Object3d.getObject("Cube");
light = Object3d.getObject("Light")

light = OLight(light)
light.maxBrightness = 2.0
light.speed = 0.01

function onSceneUpdate()
    sprite:rotate(2)
    button:rotate(2)

    cube:rotate({0,1,0}, 2)

    if isKeyPressed("RIGHT") then
	tile:attachAnimation(walkAnimation)
	tile:translate(2,0)
    elseif isKeyPressed("LEFT") then
	tile:attachAnimation(walkAnimation)
	tile:translate(-2,0)
    else
	tile:attachAnimation(idleAnimation)
    end

    local intensity = light:getIntensity()
    if intensity >= light.maxBrightness or intensity <= 0 then
	light.speed = light.speed * -1
    end

    light:setIntensity(light:getIntensity() + light.speed)
    tile:update()
end
