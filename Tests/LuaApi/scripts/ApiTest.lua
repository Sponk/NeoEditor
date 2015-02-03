
--dofile("LuaApiPongTest.lua")
--dofile("LuaApiSDKTest.lua")

light = getObject("Light");
function onSceneUpdate()
    if isKeyPressed("LEFT") then
	translate(light, {-1,0,0})
    elseif isKeyPressed("RIGHT") then
	translate(light, {1,0,0})
    end

    if isKeyPressed("UP") then
	translate(light, {0,0,1})
    elseif isKeyPressed("DOWN") then
	translate(light, {0,0,-1})
    end

end
