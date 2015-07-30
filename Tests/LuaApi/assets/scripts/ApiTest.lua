
--dofile("LuaApiPongTest.lua")
-- dofile("LuaApiSDKTest.lua")
-- dofile("LuaSimpleCollisionDetectionTest.lua")

-- dofile("Benchmark.lua")
-- dofile("NativeSDKTest.lua")

function buttonCallback()
        print("CREATING MENU")
	menu:show()
end

function quitApplication()
	NeoLua.NeoEngine.getInstance():setActive(false)
	--os.exit(0)
end

local engine = NeoLua.NeoEngine.getInstance()
local input = engine:getInputContext()
local scene = engine:getLevel():getCurrentScene()
local system = engine:getSystemContext()
local input = engine:getInputContext()

local cam = scene:getObjectByName("Light")
local teapot = scene:getObjectByName("Teapot")
local cube = scene:getObjectByName("Cube")

function flipOrthoCallback()
	local camera = scene:getCurrentCamera()
	camera:enableOrtho(not camera:isOrtho())
end

function setFovCallback()
	local camera = scene:getCurrentCamera()
	camera:setFov(tonumber(settingsWindow.inputField:getLabel()))
end

function update(delta)
        cube:rotate(NeoLua.Vector3(0,0,1), 30*delta)
        cube:rotate(NeoLua.Vector3(1,0,0), 30*delta)
        
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
