
--dofile("LuaApiPongTest.lua")
-- dofile("LuaApiSDKTest.lua")
-- dofile("LuaSimpleCollisionDetectionTest.lua")

-- dofile("Benchmark.lua")
-- dofile("NativeSDKTest.lua")

require("NeoLua")

local engine2d = NeoLua.Neo2DEngine.getInstance()
engine2d:setEnabled(true)

engine2d:setDefaultFontSize(16)
inputField = NeoLua.ThemedInputField(10, 40, 200, 20, "");
button = NeoLua.ThemedButton(10, 10, 120, 20, "This is a button!")
label = NeoLua.Label(10, 70, 0, 30, "This is a label!")

container = NeoLua.Window(500,100,400,300,"Window title!")
container:addWidget(button)
container:addWidget(inputField)
container:addWidget(label)

containerhandle = engine2d:addWidget(container)

canvas = engine2d:getCanvas(0)
canvas:addWidget(containerhandle)

button:setFontSize(14)

function buttonCallback()
	print("Callback!")
	inputField:setLabel(inputField:getLabel() .. " BUTTON PRESS ")
end
button:setScriptCallback("buttonCallback")

local engine = NeoLua.NeoEngine.getInstance()
local input = engine:getInputContext()
local scene = engine:getLevel():getCurrentScene()

local cam = scene:getObjectByName("Light")
local teapot = scene:getObjectByName("Teapot")
local sphere = scene:getObjectByName("Sphere")

function onSceneUpdate(delta)

	sphere:rotate(NeoLua.Vector3(0,0,1), 10*delta)
	teapot:rotate(NeoLua.Vector3(0,0,1), 10*delta)

	if input:isKeyPressed("UP") then
		cam:setPosition(cam:getPosition() + NeoLua.Vector3(0,0,1))
	end

	if input:isKeyPressed("DOWN") then
		cam:setPosition(cam:getPosition() + NeoLua.Vector3(0,0,-1))
	end

	if input:isKeyPressed("LEFT") then
		cam:setPosition(cam:getPosition() + NeoLua.Vector3(-1,0,0))
	end

	if input:isKeyPressed("RIGHT") then
		cam:setPosition(cam:getPosition() + NeoLua.Vector3(1,0,0))
	end
end
