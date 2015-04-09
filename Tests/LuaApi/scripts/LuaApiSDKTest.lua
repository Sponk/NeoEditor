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
    tile:setVisible(not tile:isVisible())
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
light.speed = 1


local spriteSize = getSpriteSize(sprite.widget)
print("Sprite has dimensions of " .. spriteSize[1] .. "x" .. spriteSize[2])


--- Themed button!
local themedButton = ThemedButton(20,700,200,40,"This is a button!", "btnCallback")
mainCanvas:addWidget(themedButton)

local spriteBatch = SpriteBatch()
mainCanvas:addBatch(spriteBatch)

local bsprite = spriteBatch:createSprite("maps/neo-icon.png")
bsprite:translate(100, 0)
bsprite:update()

function onSceneUpdate(delta)

    sprite:rotate(100*delta)
    button:rotate(100*delta)

    cube:rotate({0,1,0}, 100*delta)

	if isKeyPressed("JOY1_BUTTON_A") then print ("BUTTON!") end

    if isKeyPressed("RIGHT") or isKeyPressed("JOY1_BUTTON_DPADRIGHT") or getAxis("JOY1_AXIS_LEFTX") > 0.2 then
	tile:attachAnimation(walkAnimation)
	tile:translate(2,0)
    elseif isKeyPressed("LEFT") or getAxis("JOY1_AXIS_LEFTX") < -0.2 then
	tile:attachAnimation(walkAnimation)
	tile:translate(-2,0)
    else
	tile:attachAnimation(idleAnimation)
    end

    local intensity = light:getIntensity()
    if intensity >= light.maxBrightness or intensity <= 0 then
	light.speed = light.speed * -1
    end

    light:setIntensity(light:getIntensity() + light.speed * delta)
    tile:update()
end
