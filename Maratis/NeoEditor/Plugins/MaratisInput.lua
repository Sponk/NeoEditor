 
setPluginName("MaratisInput")
setPluginAuthor("Yannick Pflanzer")
setPluginLicense("")
setPluginDescription("Input method which mimics the default input method of the original Maratis editor")
enableInputMethod("update_input")

xpos = getAxis("MOUSE_X")
ypos = getAxis("MOUSE_Y")
mwheel = getAxis("MOUSE_WHEEL")

function mouse_input(camera)

end

function selection_center()
    local selection = getCurrentSelection()
    local position = vec3(0,0,0)
    
    for i = 1, #selection, 1 do
        position = position + getPosition(selection[i])
    end
    
    return position/#selection
end

function zoom_vue(camera)

    local position = getPosition(camera)
    local axis = getRotatedVector(camera, {0,0,-1})

    local wheel = getAxis("MOUSE_WHEEL") * 30.0
    local factor = length(selection_center() - position) * 0.01
    
    setPosition(camera, position - (axis * (wheel - mwheel) * factor)*getTranslationSpeed())
    mwheel = wheel
end

function pan_vue(camera)

    local xAxis = getRotatedVector(camera, {1,0,0})
    local zAxis = getRotatedVector(camera, {0,1,0})
    
    local position = getPosition(camera)
    local axis = getRotatedVector(camera, {0,0,-1})
    
    local z = dot(selection_center() - position, axis)
    local fovFactor = getCameraFov(camera) * 0.0192;
    
    mx = getAxis("MOUSE_X")
    my = getAxis("MOUSE_Y")
    
    local translationSpeed = -getTranslationSpeed()
    
    setPosition(camera, position - (xAxis * -(mx - xpos)*z*fovFactor)*translationSpeed + (zAxis * -(my - ypos)*z*fovFactor)*translationSpeed)

end

function rotate_vue(camera)
    local center = getInverseVector(camera, selection_center())
    local position = getPosition(camera)
    local rotationSpeed = getRotationSpeed()
    
    setPosition(camera, selection_center())
    
    setRotation(camera, getRotation(camera) + {-(getAxis("MOUSE_Y") - ypos)*150*rotationSpeed, 0, -(getAxis("MOUSE_X") - xpos)*150*rotationSpeed})
    --rotate(camera, getRotatedVector(camera, {0,1,0}), -(getAxis("MOUSE_X") - xpos)*150*rotationSpeed)
    --rotate(camera, getRotatedVector(camera, {1,0,0}), -(getAxis("MOUSE_Y") - ypos)*150*rotationSpeed)
    
    updateMatrix(camera)
    
    setPosition(camera, position)
    updateMatrix(camera)

    center = getTransformedVector(camera, center)
    translate(camera, (selection_center() - center))
end

function update_input()
    camera = getPerspectiveVue()

    zoom_vue(camera)
    
    if isKeyPressed("MOUSE_BUTTON_MIDDLE") and isKeyPressed("LSHIFT") then
        pan_vue(camera)
    elseif isKeyPressed("MOUSE_BUTTON_MIDDLE") then
        rotate_vue(camera)
    end
    
    xpos = getAxis("MOUSE_X")
    ypos = getAxis("MOUSE_Y")
    updateMatrix(camera)
end