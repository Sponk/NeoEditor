
--dofile("LuaApiPongTest.lua")
-- dofile("LuaApiSDKTest.lua")
-- dofile("LuaSimpleCollisionDetectionTest.lua")

-- dofile("Benchmark.lua")
-- dofile("NativeSDKTest.lua")

require("NeoEngineLua")
NeoLua = NeoEngineLua

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

function centerCursor() end

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

local mx = 0 -- input:getAxis("MOUSE_X")
local my = 0 -- input:getAxis("MOUSE_Y")

function update(delta)
        cube:rotate(NeoLua.Vector3(0,0,1), 30*delta)
        cube:rotate(NeoLua.Vector3(1,0,0), 30*delta)
        
        teapot:rotate(NeoLua.Vector3(0,0,1), 10*delta)

        if input:isKeyDown(NeoLua.KEY_UP_ARROW) then
                light:setPosition(light:getPosition() + NeoLua.Vector3(0,0,1))
        end

        if input:isKeyDown(NeoLua.KEY_DOWN_ARROW) then
                light:setPosition(light:getPosition() + NeoLua.Vector3(0,0,-1))
        end

        if input:isKeyDown(NeoLua.KEY_LEFT_ARROW) then
                light:setPosition(light:getPosition() + NeoLua.Vector3(-1,0,0))
        end

        if input:isKeyDown(NeoLua.KEY_RIGHT_ARROW) then
                light:setPosition(light:getPosition() + NeoLua.Vector3(1,0,0))
        end

        if input:isKeyDown(NeoLua.KEY_W) then
            camera:translate(NeoLua.Vector3(0,0,-100*delta), true)
        elseif input:isKeyDown(NeoLua.KEY_S) then
            camera:translate(NeoLua.Vector3(0,0,100*delta), true)
        end

        if input:isKeyDown(NeoLua.KEY_A) then
            camera:translate(NeoLua.Vector3(-100*delta,0,0), true)
        elseif input:isKeyDown(NeoLua.KEY_D) then
            camera:translate(NeoLua.Vector3(100*delta,0,0), true)
        end

        if input:isKeyDown(NeoLua.KEY_E) then
                camera:rotate(NeoLua.Vector3(0, 0, -1), 100*delta, false)
        elseif input:isKeyDown(NeoLua.KEY_Q) then
                camera:rotate(NeoLua.Vector3(0, 0, 1), 100*delta, false)
        end

        local dx = 0 -- input:getAxis("MOUSE_X")
        local dy = 0 --input:getAxis("MOUSE_Y")

        camera:rotate(NeoLua.Vector3(-1, 0, 0), (dy-my)*100, false)
        camera:rotate(NeoLua.Vector3(0, -1, 0), (dx-mx)*100, false)

        --local rotation = camera:getEulerRotation()
        --rotation.x = clamp(rotation.x, 0, 180)

        --camera:setEulerRotation(rotation)

		centerCursor()

        mx = 0 -- input:getAxis("MOUSE_X")
        my = 0 -- input:getAxis("MOUSE_Y")

        if input:onKeyDown(NeoLua.KEY_ESCAPE) then
                NeoLua.engine:setActive(false)
        end
end

function draw()

end
