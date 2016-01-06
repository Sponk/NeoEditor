
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

local light = scene:getObjectByName("Light")
local teapot = scene:getObjectByName("Teapot")
local cube = scene:getObjectByName("Cube")
local camera = scene:getCurrentCamera()

function flipOrthoCallback()
	local camera = scene:getCurrentCamera()
	camera:enableOrtho(not camera:isOrtho())
end

function setFovCallback()
	local camera = scene:getCurrentCamera()
	camera:setFov(tonumber(settingsWindow.inputField:getLabel()))
end

local function clamp(val, min, max)
        if val < min then return min elseif val > max then return max else return val end
end

centerCursor()

local mx = input:getAxis("MOUSE_X")
local my = input:getAxis("MOUSE_Y")

function update(delta)
        cube:rotate(NeoLua.Vector3(0,0,1), 30*delta)
        cube:rotate(NeoLua.Vector3(1,0,0), 30*delta)
        
        teapot:rotate(NeoLua.Vector3(0,0,1), 10*delta)

        if input:isKeyPressed("UP") then
                light:setPosition(light:getPosition() + NeoLua.Vector3(0,0,1))
        end

        if input:isKeyPressed("DOWN") then
                light:setPosition(light:getPosition() + NeoLua.Vector3(0,0,-1))
        end

        if input:isKeyPressed("LEFT") then
                light:setPosition(light:getPosition() + NeoLua.Vector3(-1,0,0))
        end

        if input:isKeyPressed("RIGHT") then
                light:setPosition(light:getPosition() + NeoLua.Vector3(1,0,0))
        end

        if input:isKeyPressed("W") then
            camera:translate(NeoLua.Vector3(0,0,-100*delta), true)
        elseif input:isKeyPressed("S") then
            camera:translate(NeoLua.Vector3(0,0,100*delta), true)
        end

        if input:isKeyPressed("A") then
            camera:translate(NeoLua.Vector3(-100*delta,0,0), true)
        elseif input:isKeyPressed("D") then
            camera:translate(NeoLua.Vector3(100*delta,0,0), true)
        end

        if input:isKeyPressed("E") then
                camera:rotate(NeoLua.Vector3(0, 0, -1), 100*delta, false)
        elseif input:isKeyPressed("Q") then
                camera:rotate(NeoLua.Vector3(0, 0, 1), 100*delta, false)
        end

        local dx = input:getAxis("MOUSE_X")
        local dy = input:getAxis("MOUSE_Y")

        camera:rotate(NeoLua.Vector3(-1, 0, 0), (dy-my)*100, false)
        camera:rotate(NeoLua.Vector3(0, -1, 0), (dx-mx)*100, false)

        --local rotation = camera:getEulerRotation()
        --rotation.x = clamp(rotation.x, 0, 180)

        --camera:setEulerRotation(rotation)

        centerCursor()

        mx = input:getAxis("MOUSE_X")
        my = input:getAxis("MOUSE_Y")

        if input:onKeyDown("ESCAPE") then
                NeoLua.engine:setActive(false)
        end
end
