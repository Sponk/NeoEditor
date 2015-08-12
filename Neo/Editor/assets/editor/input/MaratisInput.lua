
local xpos = getAxis("MOUSE_X")
local ypos = getAxis("MOUSE_Y")
local mwheel = getAxis("MOUSE_WHEEL")

local function mouse_input(camera)

end

function selection_center()
    local selection = Editor.currentSelection -- getCurrentSelection()
    local position = NeoLua.Vector3(0,0,0)
        
    for i = 1, #selection, 1 do
        position = position + selection[i]:getPosition()
    end
    
    return position/#selection
end

function zoom_vue(camera)

    local position = camera:getPosition()
    local axis = camera:getRotatedVector(NeoLua.Vector3(0,0,-1))

    local wheel = NeoLua.input:getAxis("MOUSE_WHEEL") * -30.0
    local factor = (selection_center() - position):getLength() * 0.01

    camera:setPosition(position - (axis * wheel * factor))
end

function pan_vue(camera)

    local xAxis = camera:getRotatedVector(NeoLua.Vector3(1,0,0))
    local zAxis = camera:getRotatedVector(NeoLua.Vector3(0,1,0))
    
    local position = camera:getPosition()
    local axis = camera:getRotatedVector(NeoLua.Vector3(0,0,-1))
    
    local z = (selection_center() - position):dotProduct(axis)
    local fovFactor = camera:getFov() * 0.0192;
    
    mx = NeoLua.input:getAxis("MOUSE_X")
    my = NeoLua.input:getAxis("MOUSE_Y")
    
    local translationSpeed = -1    
    camera:setPosition(position - (xAxis * -(mx - xpos)*z*fovFactor)*translationSpeed + (zAxis * -(my - ypos)*z*fovFactor)*translationSpeed)
end

function rotate_vue(camera)
    local center = camera:getInversePosition(selection_center())
    local position = camera:getPosition()
    local rotationSpeed = 1

    camera:setPosition(selection_center())
    
    camera:setEulerRotation(camera:getEulerRotation() + NeoLua.Vector3(-(NeoLua.input:getAxis("MOUSE_Y") - ypos)*150*rotationSpeed, 0, -(NeoLua.input:getAxis("MOUSE_X") - xpos)*150*rotationSpeed))
    --rotate(camera, getRotatedVector(camera, {0,1,0}), -(getAxis("MOUSE_X") - xpos)*150*rotationSpeed)
    --rotate(camera, getRotatedVector(camera, {1,0,0}), -(getAxis("MOUSE_Y") - ypos)*150*rotationSpeed)
    
    camera:updateMatrix()
    
    camera:setPosition(position)
    camera:updateMatrix()

    center = camera:getTransformedVector(center)
    camera:setPosition(camera:getPosition() + (selection_center() - center))
end

function update_input()
    camera = NeoLua.level:getCurrentScene():getCurrentCamera()
    zoom_vue(camera)
      
    if NeoLua.input:isKeyPressed("MOUSE_BUTTON_MIDDLE") and NeoLua.input:isKeyPressed("LSHIFT") then
        pan_vue(camera)
    elseif NeoLua.input:isKeyPressed("MOUSE_BUTTON_MIDDLE") then
        rotate_vue(camera)
    end
    
    xpos = NeoLua.input:getAxis("MOUSE_X")
    ypos = NeoLua.input:getAxis("MOUSE_Y")
    camera:updateMatrix()
end

return update_input
