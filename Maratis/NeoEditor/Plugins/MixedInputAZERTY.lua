 
setPluginName("MixedInputAZERTY")
setPluginAuthor("Yannick Pflanzer")
setPluginLicense("")
setPluginDescription("Mixed input method using QZSD and classic Maratis rotation")
enableInputMethod("update_input")

xpos = getAxis("MOUSE_X")
ypos = getAxis("MOUSE_Y")
mwheel = getAxis("MOUSE_WHEEL")

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

function rotate_vue(camera)
    local center = getInverseVector(camera, selection_center())
    local position = getPosition(camera)
    local rotationSpeed = getRotationSpeed()
    
    setPosition(camera, selection_center())
    
    setRotation(camera, getRotation(camera) + {-(getAxis("MOUSE_Y") - ypos)*150*rotationSpeed, 0, -(getAxis("MOUSE_X") - xpos)*150*rotationSpeed})   
    updateMatrix(camera)
    
    setPosition(camera, position)
    updateMatrix(camera)

    center = getTransformedVector(camera, center)
    translate(camera, (selection_center() - center))
end

function update_input()
    camera = getPerspectiveVue()
    translationSpeed = getTranslationSpeed()

    if isKeyPressed("Z") then
        translate(camera, {0,0,-1.0*translationSpeed}, "local")
        updateMatrix(camera)
    elseif isKeyPressed("S") then
        translate(camera, {0,0,1.0*translationSpeed}, "local")
        updateMatrix(camera)
    end
    
    if isKeyPressed("Q") then
        translate(camera, {-1.0*translationSpeed,0,0}, "local")
        updateMatrix(camera)
    elseif isKeyPressed("D") then
        translate(camera, {1.0*translationSpeed,0,0}, "local")
        updateMatrix(camera)
    end
    
    if isKeyPressed("E") then
        translate(camera, {0,1.0*translationSpeed,0}, "local")
        updateMatrix(camera)
    elseif isKeyPressed("C") then
        translate(camera, {0,-1.0*translationSpeed,0}, "local")
        updateMatrix(camera)
    end
    
    if isKeyPressed("MOUSE_BUTTON_MIDDLE") then
        rotate_vue(camera)
    end
    
    zoom_vue(camera)
    
    xpos = getAxis("MOUSE_X")
    ypos = getAxis("MOUSE_Y")  
    updateMatrix(camera)
end