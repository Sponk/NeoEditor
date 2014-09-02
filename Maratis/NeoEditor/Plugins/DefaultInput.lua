 
setPluginName("DefaultInput")
setPluginAuthor("Yannick Pflanzer")
setPluginLicense("")
setPluginDescription("Default input method using WASD")
enableInputMethod("update_input")

xpos = getAxis("MOUSE_X")
ypos = getAxis("MOUSE_Y")

function mouse_input(camera)
    local rotationSpeed = getRotationSpeed()
    local resolution = getWindowScale()
    
    mx = getAxis("MOUSE_X")
    my = getAxis("MOUSE_Y")  
    
    if isKeyPressed("MOUSE_BUTTON_RIGHT") then
        vector = vec3((ypos-my)*resolution[1], 0, (xpos-mx)*resolution[2])      
        setRotation(camera, getRotation(camera) + (vector*rotationSpeed))   
        updateMatrix(camera)
    end
    
    xpos = mx
    ypos = my
end

function update_input()
    camera = getPerspectiveVue()
    translationSpeed = getTranslationSpeed()

    if isKeyPressed("W") then
        translate(camera, {0,0,-1.0*translationSpeed}, "local")
        updateMatrix(camera)
    elseif isKeyPressed("S") then
        translate(camera, {0,0,1.0*translationSpeed}, "local")
        updateMatrix(camera)
    end
    
    if isKeyPressed("A") then
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
    
    mouse_input(camera)
end