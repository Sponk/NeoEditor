
local xpos = getAxis("MOUSE_X")
local ypos = getAxis("MOUSE_Y")

local function mouse_input(camera)

end

function zoom_vue(camera)

    local position = camera:getPosition()
    local axis = camera:getRotatedVector(NeoLua.Vector3(0,0,-1))

    local wheel = NeoLua.input:getAxis("MOUSE_WHEEL") * -30.0
    local factor = (Editor.getSelectionCenter() - position):getLength() * 0.01

    camera:setPosition(position - (axis * wheel * factor))
end

function pan_vue(camera)

    local xAxis = camera:getRotatedVector(NeoLua.Vector3(1,0,0))
    local zAxis = camera:getRotatedVector(NeoLua.Vector3(0,1,0))
    
    local position = camera:getPosition()
    local axis = camera:getRotatedVector(NeoLua.Vector3(0,0,-1))
    
    local z = (Editor.getSelectionCenter() - position):dotProduct(axis)
    local fovFactor = camera:getFov() * 0.0192;
    
    local mx = NeoLua.input:getAxis("MOUSE_X")
    local my = NeoLua.input:getAxis("MOUSE_Y")
    
    local translationSpeed = -1    
    camera:setPosition(position - (xAxis * -(mx - xpos)*z*fovFactor)*translationSpeed + (zAxis * -(my - ypos)*z*fovFactor)*translationSpeed)
end

function rotate_vue(camera)
    local selectionCenter = Editor.getSelectionCenter()
    local center = camera:getInversePosition(selectionCenter)
    local position = camera:getPosition()
    local rotationSpeed = 1

    camera:setPosition(selectionCenter)
    
    camera:setEulerRotation(camera:getEulerRotation() + NeoLua.Vector3(-(NeoLua.input:getAxis("MOUSE_Y") - ypos)*150*rotationSpeed, 0, -(NeoLua.input:getAxis("MOUSE_X") - xpos)*150*rotationSpeed))
    --rotate(camera, getRotatedVector(camera, {0,1,0}), -(getAxis("MOUSE_X") - xpos)*150*rotationSpeed)
    --rotate(camera, getRotatedVector(camera, {1,0,0}), -(getAxis("MOUSE_Y") - ypos)*150*rotationSpeed)
    
    camera:updateMatrix()
    
    camera:setPosition(position)
    camera:updateMatrix()

    center = camera:getTransformedVector(center)
    camera:setPosition(camera:getPosition() + (selectionCenter - center))
end

function update_input()
    local camera = NeoLua.level:getCurrentScene():getCurrentCamera()
    zoom_vue(camera)
      
    if NeoLua.input:isKeyPressed("MOUSE_BUTTON_MIDDLE") and NeoLua.input:isKeyPressed("LSHIFT") then
        pan_vue(camera)
    elseif NeoLua.input:isKeyPressed("MOUSE_BUTTON_MIDDLE") then
        rotate_vue(camera)
    end
    
    xpos = NeoLua.input:getAxis("MOUSE_X")
    ypos = NeoLua.input:getAxis("MOUSE_Y")

    if NeoLua.input:isKeyPressed("MOUSE_BUTTON_MIDDLE") then
        local res = NeoLua.system:getScreenSize()
        local mx = xpos * res.x
        local my = ypos * res.y

        if mx > res.x - 10 then
            NeoLua.system:setCursorPosition(10, my)
            xpos = 0
        elseif mx < 10 then
            NeoLua.system:setCursorPosition(res.x - 10, my)
            xpos = 1
        end

        if my > res.y - 10 then
            NeoLua.system:setCursorPosition(mx, 10)
            ypos = 0
        elseif my < 10 then
            NeoLua.system:setCursorPosition(mx, res.y - 11)
            ypos = 1
        end

    end

    camera:updateMatrix()
end

return update_input
