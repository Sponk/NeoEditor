--- main.lua - The editor main script
-- This script contains most of the logic that drives the scene editor including
-- input management and translations. It is built in a way that allows suspending the whole UI,
-- deleting it from memory and reloading it on the fly for playing the game in the editor.

Editor = {}

Shortcuts = require("editor.shortcuts")

require("editor.callbacks")
local lfs = require("lfs")

--- Loads the complete UI into Neo2D and displays it.
-- Needs Editor.setupTranslatedUI to be called first so the GUI description is loaded with the
-- right translation.
function Editor.loadUI()
    Editor.sceneDlg = Editor.dlgTables.sceneDlg.create()
    Editor.updateSceneTree()

    Gui.loadFromTable(Editor.dlgTables.menubarTable)

    -- This one sets itself up
    Editor.aboutDlg = Editor.dlgTables.aboutDlg.create()
    -- Editor.aboutDlg = Gui.loadFromTable(aboutDlgTable)

    Editor.entityEditor = Editor.dlgTables.entityEditor.create()
    Editor.lightEditor = Editor.dlgTables.lightEditor.create()
    Editor.textEditor = Editor.dlgTables.textEditor.create()
    Editor.soundEditor = Editor.dlgTables.soundEditor.create()
    Editor.cameraEditor = Editor.dlgTables.cameraEditor.create()

    Editor.toolbar = Editor.dlgTables.toolbar.create()

    Gui.wm:selectWindow(Editor.sceneDlg["window"].window)
end

--- Adds the given object and all child objects to the given tree node
-- to mirror the scene tree.
--
-- This function is recursive and walks down the tree until no child is left to add.
--
-- @param obj The object to add.
-- @param node The node to use as the root node.
function Editor.addChildrenToTree(obj, node)
    local sz = obj:getChildrenNumber()

    for i = 0, sz - 1, 1 do
        local child = obj:getChild(i)
        local childNode = node:addChild(child:getName())
        Editor.addChildrenToTree(child, childNode)
    end
end

--- Update the scene tree by clearing it first and then rebuilding the tree from
-- the current scene tree.
function Editor.updateSceneTree()
    local scene = NeoLua.level:getCurrentScene()
    local sz = scene:getObjectsNumber()

    local tree = Editor.sceneDlg["window"]["layout"]["scrollpanel"]["tree"]
    tree:clear()

    local node = tree:getTreeModel()

    for i = 0, sz - 1, 1 do
        local object = scene:getObjectByIndex(i)

        if not object:hasParent() and object:getId() ~= Editor.sceneCamera:getId() then
            if object:getName():len() > 0 then
                local childNode = node:addChild(object:getName())
                Editor.addChildrenToTree(object, childNode)
            end
        end
    end
end

--- Loads the list of available translations from "./translations"
-- while using LFS for iterating over those files.
-- Translations are automatically added to the translator with
-- "Translator.addTranslation" and can be used directly.
--
-- FIXME: Should take the translator as an argument!
function Editor.loadTranslationList()
    -- Load list of translations
    local iter, dir_obj = lfs.dir("./translations")
    local v = iter(dir_obj)
    local translationsList = ""
    while v ~= nil do
        -- Check if we actually found some csv formatted file
        if v:find(".csv") ~= nil then
            translationsList = translationsList .. v .. " "
            Translator.addTranslation(v, "./translations/" .. v)
        end

        v = iter(dir_obj)
    end

    infoLog("Found editor translations: " .. translationsList)
end

--- Loads all available input methods from "assets/editor/input/" and
-- registers them with the input system. The default method chosen is usually
-- "MaratisInput.lua"
--
-- FIXME: Should return the list as a value, not manipulate it directly!
function Editor.loadInputSystem()
    -- Load list of input methods
    Editor.inputMethods = {}
    local path = "input/"
    local iter, dir_obj = lfs.dir("assets/editor/input/")
    local v = iter(dir_obj)
    local inputList = ""
    while v ~= nil do
        -- Check if we actually found some csv formatted file
        if v:find(".lua") ~= nil then
            inputList = inputList .. v .. " "
            Editor.inputMethods[v] = path .. v
        end

        v = iter(dir_obj)
    end

    infoLog("Found input methods: " .. inputList)
end

--- Loads handle meshes with three components, customizes their color and
-- adds them to the given scene. The handles will be invisible by default and
-- colored in red, green and blue.
--
-- @param filename The mesh file to load.
-- @param scene The scene to add the handles to.
-- @return A table with the new entities as x, y and z components.
function Editor.loadHandleMesh(filename, scene)
    local model = NeoLua.level:loadMesh(filename)

    if model == nil then return nil end

    local x = scene:addNewEntity(model)
    local y = scene:addNewEntity(model)
    local z = scene:addNewEntity(model)

    x:getMaterial(0):setEmit(NeoLua.Vector3(1, 0, 0))
    y:getMaterial(0):setEmit(NeoLua.Vector3(0, 1, 0))
    z:getMaterial(0):setEmit(NeoLua.Vector3(0, 0, 1))

    x:setEulerRotation(NeoLua.Vector3(0, 90, 0))
    y:setEulerRotation(NeoLua.Vector3(90, 0, 0))

    x:setActive(false)
    y:setActive(false)
    z:setActive(false)

    return {x = x, y = y, z = z}
end

--- Loads a new billboard to the overlay scene and sets up all
-- behaviors needed for it to function properly.
--
-- @param object The parent object of the billboard.
-- @param modelpath The path to the billboard to use.
-- @return The billboard entity.
function Editor.addObjectBillboard(object, modelpath)
    local bmanager = NeoLua.engine:getBehaviorManager()
    local lookAtCreator = bmanager:getBehaviorByName("LookAt")
    local followCreator = bmanager:getBehaviorByName("Follow")

    local model = NeoLua.level:loadMesh(modelpath)
    local scene = NeoLua.level:getCurrentScene()

    local entity = Editor.overlayScene:addNewEntity(model)
    local behavior = lookAtCreator:getNewBehavior(entity)
    entity:addBehavior(behavior)

    local var = behavior:getVariable(0)
    local str = NeoLua.Voidp2String(var:getPointer())
    str:set("MainSceneCamera")

    behavior = followCreator:getNewBehavior(entity)
    entity:addBehavior(behavior)

    var = behavior:getVariable(0)
    str = NeoLua.Voidp2String(var:getPointer())
    str:set(object:getName())

    local delay = NeoLua.floatp.frompointer(NeoLua.Voidp2Float(behavior:getVariable(1):getPointer()))
    delay:assign(0)

    entity:setName(object:getName())
    return entity;
end

--- Loads all meshes needed for displaying input handles and similar into
-- the current level. This also includes adding them to the new scene.
function Editor.loadMeshes()
    Editor.sceneMeshes = {
        translation = Editor.loadHandleMesh("assets/editor/meshes/arrow.obj", Editor.overlayScene),
        rotation = Editor.loadHandleMesh("assets/editor/meshes/torus.obj", Editor.overlayScene),
        scale = Editor.loadHandleMesh("assets/editor/meshes/scale-arrow.obj", Editor.overlayScene),
    }

    local level = NeoLua.level
    Editor.objectMeshes = {}
    
    Editor.objectMeshes.sphere = Editor.overlayScene:addNewEntity(level:loadMesh("assets/editor/meshes/sphere.obj"))
    Editor.objectMeshes.sphere:enableWireframe(true)
    Editor.objectMeshes.sphere:setActive(false)
    
    Editor.objectMeshes.cone = Editor.overlayScene:addNewEntity(level:loadMesh("assets/editor/meshes/cone.obj"))   
    Editor.objectMeshes.cone:enableWireframe(true)
    Editor.objectMeshes.cone:setActive(false)
    
    Editor.translationMode = Editor.sceneMeshes.translation

    local bmanager = NeoLua.engine:getBehaviorManager()
    local lookAtCreator = bmanager:getBehaviorByName("LookAt")
    local followCreator = bmanager:getBehaviorByName("Follow")

    Editor.cameraBillboards = {}
    local model = NeoLua.level:loadMesh("assets/editor/meshes/objects/camera.dae")
    local scene = NeoLua.level:getCurrentScene()
    for i = 0, scene:getCamerasNumber() - 1, 1 do

        local camera = scene:getCameraByIndex(i)
        if camera:getName() ~= "MainSceneCamera" then

            local entity = Editor.overlayScene:addNewEntity(model)
            local behavior = lookAtCreator:getNewBehavior(entity)
            entity:addBehavior(behavior)

            local var = behavior:getVariable(0)
            local str = NeoLua.Voidp2String(var:getPointer())
            str:set("MainSceneCamera")

            behavior = followCreator:getNewBehavior(entity)
            entity:addBehavior(behavior)

            var = behavior:getVariable(0)
            str = NeoLua.Voidp2String(var:getPointer())
            str:set(camera:getName())

            local delay = NeoLua.floatp.frompointer(NeoLua.Voidp2Float(behavior:getVariable(1):getPointer()))
            delay:assign(0)

            entity:setName(camera:getName())
            table.insert(Editor.cameraBillboards, entity)
        end
    end

    Editor.lightBillboards = {}
    local model = NeoLua.level:loadMesh("assets/editor/meshes/objects/light.dae")
    for i = 0, scene:getLightsNumber() - 1, 1 do

        local light = scene:getLightByIndex(i)
        local entity = Editor.overlayScene:addNewEntity(model)
        local behavior = lookAtCreator:getNewBehavior(entity)
        entity:addBehavior(behavior)

        local var = behavior:getVariable(0)
        local str = NeoLua.Voidp2String(var:getPointer())
        str:set("MainSceneCamera")

        behavior = followCreator:getNewBehavior(entity)
        entity:addBehavior(behavior)

        var = behavior:getVariable(0)
        str = NeoLua.Voidp2String(var:getPointer())
        str:set(light:getName())

        local delay = NeoLua.floatp.frompointer(NeoLua.Voidp2Float(behavior:getVariable(1):getPointer()))
        delay:assign(0)

        entity:setName(light:getName())
        table.insert(Editor.lightBillboards, entity)
    end

    Editor.soundBillboards = {}
    local model = NeoLua.level:loadMesh("assets/editor/meshes/objects/sound.dae")
    for i = 0, scene:getSoundsNumber() - 1, 1 do

        local sound = scene:getSoundByIndex(i)
        local entity = Editor.overlayScene:addNewEntity(model)
        local behavior = lookAtCreator:getNewBehavior(entity)
        entity:addBehavior(behavior)

        local var = behavior:getVariable(0)
        local str = NeoLua.Voidp2String(var:getPointer())
        str:set("MainSceneCamera")

        behavior = followCreator:getNewBehavior(entity)
        entity:addBehavior(behavior)

        var = behavior:getVariable(0)
        str = NeoLua.Voidp2String(var:getPointer())
        str:set(sound:getName())

        local delay = NeoLua.floatp.frompointer(NeoLua.Voidp2Float(behavior:getVariable(1):getPointer()))
        delay:assign(0)

        entity:setName(sound:getName())
        table.insert(Editor.soundBillboards, entity)
    end

    --entity:enableWireframe(true)
end

--- Casts a ray starting at the given origin into the
-- given direction and returns the point and a boolean if the object was hit.
--
-- Can be used as follows:
-- \code
-- wasHit,point = Editor.castRay(entity, rayO, rayD)
-- \endcode
--
-- @param entity An OEntity object
-- @param rayO The origin of the ray
-- @param rayD The direction of the ray
-- @return {boolean, object3d}
function Editor.castRay(entity, rayO, rayD)

    if entity == nil then return unpack({ false, nil }) end;

    if entity:getType() ~= NeoLua.OBJECT3D_ENTITY then
        return
    end

    entity:getMesh():updateBoundingBox()
    local box = entity:getMesh():getBoundingBox()

    local pos = entity:getTransformedPosition()
    local iMatrix = entity:getMatrix():getInverse()

    local localRayO = iMatrix * rayO
    local localRayD = iMatrix * rayD
    if not NeoLua.isEdgeToBoxCollision(localRayO, localRayD, box.min, box.max) then
        return unpack({ false, nil })
    end

    local mesh = entity:getMesh()
    local sz = mesh:getSubMeshsNumber()

    for i = 0, sz - 1, 1 do
        local submesh = mesh:getSubMesh(i)
        local isz = submesh:getIndicesSize()
        for j = 0, isz - 1, 3 do
            local v1 = submesh:getVertex(submesh:getIndex(j))
            local v2 = submesh:getVertex(submesh:getIndex(j + 1))
            local v3 = submesh:getVertex(submesh:getIndex(j + 2))

            --print("v1: ", v1.x, v1.y, v1.z)
            --print("v2: ", v2.x, v2.y, v2.z)
            --print("v3: ", v3.x, v3.y, v3.z)

            local ipoint = NeoLua.Vector3()

            -- bool isEdgeTriangleIntersection(const Vector3 & origin, const Vector3 & dest, const Vector3 & a, const Vector3 & b, const Vector3 & c, const Vector3 & normal, Vector3 * point)
            local retval = NeoLua.isEdgeTriangleIntersection(localRayO, localRayD, v1, v2, v3, NeoLua.getTriangleNormal(v1, v2, v3), ipoint)

            if retval then
                --print("ipoint: ", ipoint.x, ipoint.y, ipoint.z)
                return unpack({ true, entity:getMatrix() * ipoint })
            end
        end
    end

    return unpack({ false, nil })
end

--- Casts a ray onto a bounding box only.
-- Can be used with any object type that defines a "getBoundingBox()" method.
--
-- @param entity An OEntity object
-- @param rayO The origin of the ray
-- @param rayD The direction of the ray
-- @param obj Any Object3d subclass that has a bounding box.
-- @return A boolean value.
function Editor.castRayOnBox(obj, rayO, rayD)
    local box = obj:getBoundingBox()
    local pos = obj:getTransformedPosition()
    local iMatrix = obj:getMatrix():getInverse()

    local localRayO = iMatrix * rayO
    local localRayD = iMatrix * rayD
    return NeoLua.isEdgeToBoxCollision(localRayO, localRayD, box.min, box.max)
end

--- Updates the current selection and translates/rotates/scales the selection
-- using the transformation handles in the scene.
function Editor.selectObject()

    local scene = NeoLua.level:getCurrentScene()
    local numObjects = scene:getEntitiesNumber()

    local res = NeoLua.system:getScreenSize()
    local mx = NeoLua.input:getAxis("MOUSE_X") * res.x
    local my = (1 - NeoLua.input:getAxis("MOUSE_Y")) * res.y
    local camera = Editor.sceneCamera

    local cameraAxis = camera:getRotatedVector(NeoLua.Vector3(0, 0, -1)):getNormalized()

    local rayO = camera:getTransformedPosition()
    local rayD = camera:getUnProjectedPoint(NeoLua.Vector3(mx, my, 1))

    if camera:isOrtho() then
        local point = NeoLua.Vector3()
        isRayPlaneIntersection(rayD, -cameraAxis, rayO + (cameraAxis * camera:getClippingNear()), cameraAxis, point)
        rayO = point
    end

    rayD = rayO + ((rayD - rayO):getNormalized() * (camera:getClippingFar() - camera:getClippingNear()));

    if NeoLua.input:isKeyPressed("MOUSE_BUTTON_LEFT") and Editor.selectedArrow ~= nil and #Editor.currentSelection == 1 then

        --infoLog("SELECTED ARROW");
        --value,point = Editor.castRay(Editor.selectedArrow, rayO, rayD)
        --if not value then return end

        local p1 = camera:getUnProjectedPoint(NeoLua.Vector3(mx, my, 0.3))
        local p2 = camera:getUnProjectedPoint(NeoLua.Vector3(Editor.mx * res.x, (1 - Editor.my) * res.y, 0.3))
        local dif = (p1 - p2) * 100
        --local dif = dif * 100 --dif:getNormalized() * dif:getLength() * 100

        -- print("mx: ", mx, my)
        -- print("omx: ", Editor.mx, Editor.my)
        -- print("p1: ", p1.x, p1.y, p1.z)
        -- print("p2: ", p2.x, p2.y, p2.z)
        -- print("dif: ", dif.x, dif.y, dif.z)

        local scale = (camera:getPosition() - Editor.currentSelection[1]:getTransformedPosition()):getLength()

        if Editor.translationMode == Editor.sceneMeshes.translation then
            Editor.currentSelection[1]:translate(Editor.selectedArrow * dif * scale * 0.7)
        elseif Editor.translationMode == Editor.sceneMeshes.scale then
            Editor.currentSelection[1]:setScale(Editor.currentSelection[1]:getScale() + Editor.selectedArrow * dif * scale * 0.007)
        elseif Editor.translationMode == Editor.sceneMeshes.rotation then
            Editor.currentSelection[1]:setEulerRotation(Editor.currentSelection[1]:getEulerRotation() + Editor.selectedArrow * dif * scale)
        end

        Editor.currentSelection[1]:updateMatrix()

        Editor.entityEditor.updateData()
        Editor.lightEditor.updateData()
        Editor.textEditor.updateData()
        Editor.soundEditor.updateData()
        Editor.cameraEditor.updateData()

        Editor.lastPoint = point
        return

        --Editor.selectedArrow:translate(Editor.lastPoint - point)
    elseif NeoLua.input:isKeyPressed("MOUSE_BUTTON_LEFT") and #Editor.currentSelection == 1 then

        local value, point = Editor.castRay(Editor.currentSelection[1], rayO, rayD)
        --if not value then return end

        --if value then

        local p1 = camera:getUnProjectedPoint(NeoLua.Vector3(mx, my, 0.3))
        local p2 = camera:getUnProjectedPoint(NeoLua.Vector3(Editor.mx * res.x, (1 - Editor.my) * res.y, 0.3))
        local dif = p1 - p2

        -- print("mx: ", mx, my)
        -- print("omx: ", Editor.mx, Editor.my)
        -- print("p1: ", p1.x, p1.y, p1.z)
        -- print("p2: ", p2.x, p2.y, p2.z)
        -- print("dif: ", dif.x, dif.y, dif.z)

        local scale = (camera:getPosition() - Editor.currentSelection[1]:getTransformedPosition()):getLength() * 100

        if Editor.translationMode == Editor.sceneMeshes.translation then
            Editor.currentSelection[1]:translate(dif * scale * 0.7)
        elseif Editor.translationMode == Editor.sceneMeshes.scale then
            Editor.currentSelection[1]:setScale(Editor.currentSelection[1]:getScale() * (1-Editor.dy))
        elseif Editor.translationMode == Editor.sceneMeshes.rotation then
            Editor.currentSelection[1]:setEulerRotation(Editor.currentSelection[1]:getEulerRotation() + dif * -scale)
        end

        Editor.currentSelection[1]:updateMatrix()

        Editor.entityEditor.updateData()
        Editor.lightEditor.updateData()
        Editor.textEditor.updateData()
        Editor.soundEditor.updateData()
        Editor.cameraEditor.updateData()

        Editor.lastPoint = point
        --end
    end

    Editor.selectedArrow = nil

    if NeoLua.input:onKeyDown("MOUSE_BUTTON_LEFT") then
        -- Check arrows
        if #Editor.currentSelection > 0 then
           --[[ if Editor.translationMode == Editor.sceneMeshes.translation then
                local meshes = Editor.sceneMeshes.translation
                if Editor.castRay(meshes.x, rayO, rayD) then
                    Editor.selectedArrow = NeoLua.Vector3(1, 0, 0)
                    return
                elseif Editor.castRay(meshes.y, rayO, rayD) then
                    Editor.selectedArrow = NeoLua.Vector3(0, 1, 0)
                    return
                elseif Editor.castRay(meshes.z, rayO, rayD) then
                    Editor.selectedArrow = NeoLua.Vector3(0, 0, 1)
                    return
                end
            elseif Editor.translationMode == Editor.sceneMeshes.rotation then
                local meshes = Editor.sceneMeshes.rotation
                if Editor.castRay(meshes.x, rayO, rayD) then
                    Editor.selectedArrow = NeoLua.Vector3(1, 0, 0)
                    return
                elseif Editor.castRay(meshes.y, rayO, rayD) then
                    Editor.selectedArrow = NeoLua.Vector3(0, 1, 0)
                    return
                elseif Editor.castRay(meshes.z, rayO, rayD) then
                    Editor.selectedArrow = NeoLua.Vector3(0, 0, 1)
                    return
                end
            end]]

           local meshes = Editor.translationMode
           if Editor.castRay(meshes.x, rayO, rayD) then
               Editor.selectedArrow = NeoLua.Vector3(1, 0, 0)
               return
           elseif Editor.castRay(meshes.y, rayO, rayD) then
               Editor.selectedArrow = NeoLua.Vector3(0, 1, 0)
               return
           elseif Editor.castRay(meshes.z, rayO, rayD) then
               Editor.selectedArrow = NeoLua.Vector3(0, 0, 1)
               return
           end
        end

        local possibleSelection = {}

        -- Check all entities
        for i = 0, numObjects - 1, 1 do
            local entity = scene:getEntityByIndex(i)
            if entity ~= nil and Editor.castRay(entity, rayO, rayD) then

                table.insert(possibleSelection, entity)
                --Editor.entityEditor.setShownObject(entity:getName())

                --return entity
            end
        end

        numObjects = scene:getTextsNumber()

        -- Check all text objects
        for i = 0, numObjects - 1, 1 do
            local entity = scene:getTextByIndex(i)
            if entity ~= nil and Editor.castRayOnBox(entity, rayO, rayD) then
                table.insert(possibleSelection, entity)
            end
        end

        -- Check all light objects
        for i,v in ipairs(Editor.lightBillboards) do
            local entity = scene:getLightByName(v:getName())
            if entity ~= nil and Editor.castRayOnBox(v, rayO, rayD) then
                table.insert(possibleSelection, entity)
            end
        end

        -- Check all sound objects
        for i,v in ipairs(Editor.soundBillboards) do
            local entity = scene:getSoundByName(v:getName())
            if entity ~= nil and Editor.castRayOnBox(v, rayO, rayD) then
                table.insert(possibleSelection, entity)
            end
        end

        -- Check all camera objects
        for i,v in ipairs(Editor.cameraBillboards) do
            local entity = scene:getCameraByName(v:getName())
            if entity ~= nil and Editor.castRayOnBox(v, rayO, rayD) then
                table.insert(possibleSelection, entity)
            end
        end

        if #possibleSelection <= 0 then Editor.select(nil) return end

        local camPos = camera:getPosition()
        local selectedEntity = possibleSelection[1]
        local lastDistance = (camPos - selectedEntity:getTransformedPosition()):getLength()
        for k, v in ipairs(possibleSelection) do
            local dst = (camPos - v:getTransformedPosition()):getLength()
            if dst < lastDistance then
                lastDistance = dst
                selectedEntity = v
            end
        end

        Editor.select(selectedEntity)
    end
end

--- Select the given Object3d and updates all dialogs and handles
-- accordingly.
--
-- @param obj The object to select.
function Editor.select(obj)

    local arrows = Editor.translationMode

    if obj == nil then

        for k, v in ipairs(Editor.currentSelection) do
            if v:getType() == NeoLua.OBJECT3D_ENTITY then
                v:enableWireframe(false)
            end
        end

        for k,arrows in pairs(Editor.sceneMeshes) do
            arrows.x:setActive(false)
            arrows.y:setActive(false)
            arrows.z:setActive(false)
        end
	
	for k,v in pairs(Editor.objectMeshes) do
            v:setActive(false)
        end

        Editor.currentSelection = {}
        return
    end

    local position = obj:getTransformedPosition()

    arrows.x:setActive(true)
    arrows.y:setActive(true)
    arrows.z:setActive(true)

    Editor.entityEditor.setShownObject(obj:getName())
    Editor.lightEditor.setShownObject(obj:getName())
    Editor.textEditor.setShownObject(obj:getName())
    Editor.soundEditor.setShownObject(obj:getName())
    Editor.cameraEditor.setShownObject(obj:getName())

    Editor.currentSelection = { obj }
    
    if obj:getType() == NeoLua.OBJECT3D_LIGHT then
	if obj:getSpotAngle()  >= 180 then
		Editor.objectMeshes.sphere:setPosition(position)
		
		local radius = obj:getRadius() * 0.05
		Editor.objectMeshes.sphere:setScale(NeoLua.Vector3(radius, radius, radius))
		Editor.objectMeshes.sphere:setActive(true)
		Editor.objectMeshes.cone:setActive(false)
	else
		Editor.objectMeshes.cone:setPosition(position)
		
		local radius = obj:getRadius() * 0.05
		
		-- Calculate lower width with trigonometry
		-- tan(alpha) = width / radius <=> radius * tan(alpha) = width
		
		local width = radius * math.tan(obj:getSpotAngle())
		Editor.objectMeshes.cone:setScale(NeoLua.Vector3(width, width, radius))
		Editor.objectMeshes.cone:setActive(true)
		
		Editor.objectMeshes.sphere:setActive(false)
	end
    else
	Editor.objectMeshes.sphere:setActive(false)
    end

    Editor.sceneDlg["window"]["layout"]["scrollpanel"]["tree"]:selectEntry(obj:getName())
end

--- Calculates the center of the current selection.
-- @return A NeoLua.Vector3
function Editor.getSelectionCenter()
    local selection = Editor.currentSelection
    local position = NeoLua.Vector3(0, 0, 0)

    for k, v in ipairs(selection) do
        position = position + v:getTransformedPosition()
    end

    return position / #selection
end

--- Updates all translation handles in the current scene.
function Editor.updateHandles()
	local arrows = Editor.translationMode
	local position = Editor.getSelectionCenter()
	local res = NeoLua.system:getScreenSize()

	local radius = (Editor.sceneCamera:getPosition() - position):getLength() * 0.005

	arrows.x:setScale(NeoLua.Vector3(radius, radius, radius))
	arrows.y:setScale(NeoLua.Vector3(radius, radius, radius))
	arrows.z:setScale(NeoLua.Vector3(radius, radius, radius))

	arrows.x:setPosition(position)
	arrows.y:setPosition(position)
	arrows.z:setPosition(position)
	
	if #Editor.currentSelection > 0 then
		if Editor.currentSelection[1]:getType() == NeoLua.OBJECT3D_LIGHT then
			local radius = Editor.currentSelection[1]:getRadius() * 0.05
			Editor.objectMeshes.sphere:setScale(NeoLua.Vector3(radius, radius, radius))
			
			local width = radius * math.tan(Editor.currentSelection[1]:getSpotAngle())
			Editor.objectMeshes.cone:setScale(NeoLua.Vector3(width, width, radius))
			
			Editor.objectMeshes.cone:setEulerRotation(Editor.currentSelection[1]:getEulerRotation())
		
			if Editor.currentSelection[1]:getSpotAngle()  >= 180 then
				Editor.objectMeshes.cone:setActive(false)
				Editor.objectMeshes.sphere:setActive(true)
			else
				Editor.objectMeshes.sphere:setActive(false)
				Editor.objectMeshes.cone:setActive(true)
			end
		end
	end
	
	Editor.objectMeshes.sphere:setPosition(position)
	Editor.objectMeshes.cone:setPosition(position)	
end

--- Quits the program
function quitCallback()
    NeoLua.engine:setActive(false)
end

--- Updates the editor and calls the game specific update function
-- when he game is running inside the editor.
function update(dt)

    if Editor.isGameRunning then
        pcall(Editor.gameUpdate, dt)
        return
    end

    if Editor.requestReload == true then
        Editor.reload()
        Editor.requestReload = false
    end

    -- Calculate mouse delta
    local mx = NeoLua.input:getAxis("MOUSE_X")
    local my = NeoLua.input:getAxis("MOUSE_Y")

    Editor.dx = mx - Editor.mx
    Editor.dy = my - Editor.my

    if NeoLua.input:isKeyPressed("MOUSE_BUTTON_MIDDLE") or not NeoLua.Neo2DEngine.getInstance():isMouseOnGui() then
        Editor.inputMethod()
    end

    if not NeoLua.Neo2DEngine.getInstance():isMouseOnGui() then
        Editor.selectObject()
    end

    Shortcuts.update()
    Editor.toolbar.update()
    Editor.updateHandles()

    Editor.mx = mx
    Editor.my = my

    -- collectgarbage("steps", 2)
end

Editor.update = update

--- Sets up the current level and scene with the right
-- editor camera and an additional scene for overlays.
function Editor.setupLevel()
    local currentScene = NeoLua.level:getCurrentScene()

    Editor.gameCamera = currentScene:getCurrentCamera()

    Editor.sceneCamera = currentScene:addNewCamera()
    Editor.overlayScene = NeoLua.level:addNewScene()
    Editor.overlayScene:setName("EditorOverlay")

    Editor.overlaySceneId = NeoLua.level:getScenesNumber()
    Editor.sceneCamera:setSceneLayer(Editor.overlaySceneId)

    Editor.sceneCamera:setName("MainSceneCamera")
    Editor.sceneCamera:setPosition(NeoLua.Vector3(0, -200, 200))
    Editor.sceneCamera:setEulerRotation(NeoLua.Vector3(90, 0, 0))

    Editor.sceneCamera:setEulerRotation(NeoLua.Vector3(40, 0, 0))

    Editor.overlayCamera = Editor.overlayScene:addNewCamera()
    Editor.overlayCamera:setName("Camera")
    Editor.overlayCamera:linkTo(Editor.sceneCamera)

    -- Set up cameras for maximum view range
    Editor.sceneCamera:setClippingFar(100000)
    Editor.sceneCamera:setClippingNear(0.01)

    Editor.overlayCamera:setClippingFar(100000)
    Editor.overlayCamera:setClippingNear(0.01)

    NeoLua.level:setCurrentScene(currentScene)
    currentScene:setCurrentCamera(Editor.sceneCamera)
end

--- Reloads the editor UI
function Editor.reload()
    NeoLua.Neo2DEngine.getInstance():clear()

    --NeoLua.engine:loadLevel(filename)
    Editor.setupLevel()

    Editor.loadMeshes()

    Gui.setupWM()
    Editor.loadUI()
    Editor.updateSceneTree()
end

--- Loads the translated UI description into memory.
-- Needs to be called before Editor.setupUI!
function Editor.setupTranslatedUI(translation)
    infoLog("Using translation: " .. translation)
    Translator.swapTranslation(translation)

    Editor.dlgTables = {
        sceneDlg = dofile("dialogs/scene-tree-desc.lua"),
        aboutDlg = dofile("dialogs/aboutdlg.lua"),
        entityEditor = dofile("dialogs/entity-editor.lua"),
        lightEditor = dofile("dialogs/light-editor.lua"),
        textEditor = dofile("dialogs/text-editor.lua"),
        soundEditor = dofile("dialogs/sound-editor.lua"),
        cameraEditor = dofile("dialogs/camera-editor.lua"),
        menubarTable = dofile("menubar.lua"),
        toolbar = dofile("toolbar.lua")
    }
end

--- Plays the game inside the editor and suspends the editor.
-- The editor needs to be reloaded after calling this.
--
-- @param mainscript The main script file to execute.
function Editor.playGame(mainscript)
    local updateBackup = update
    update = nil
    dofile(mainscript)

    if update == nil then
        update = updateBackup
        Gui.messageBox(tr("Error"), tr("Can not play the game!\nScript file \"game.lua\" not found!"))
        return
    end

    Editor.gameUpdate = update
    Editor.isGameRunning = true
    update = updateBackup

    NeoLua.Neo2DEngine.getInstance():clear()

    --Editor.sceheMeshes.arrows.x:setActive(false)
    --Editor.sceneMeshes.y:setActive(false)
    --Editor.sceneMeshes.z:setActive(false)

    NeoLua.level:getCurrentScene():setCurrentCamera(Editor.gameCamera)
end

--- Cleans up the editor data structures and removes the overlay editor scene
-- with all objects in it.
function Editor.cleanUp()
    NeoLua.level:deleteScene(Editor.overlaySceneId - 1)
    NeoLua.level:getCurrentScene():deleteObject(Editor.sceneCamera)

    Editor.sceneCamera = nil
    Editor.overlayScene = nil
    Editor.overlaySceneId = nil
    Editor.sceneMeshes = nil
    Editor.sceneCamera = nil
    Editor.overlayCamera = nil
end

function Editor.autoSave()

    Editor.cleanUp()
    NeoLua.engine:saveLevel(filename)

    Editor.setupLevel()
    Editor.loadMeshes()
end

function Editor.loadAutoSave()
end

--- Adds a new light to the current scene and selects it.
function Editor.addLight()
    local light = NeoLua.level:getCurrentScene():addNewLight()
    light:setName(findName("Light", false))
    light:setPosition(Editor.getSelectionCenter())

    local entity = Editor.addObjectBillboard(light, "assets/editor/meshes/objects/light.dae")
    table.insert(Editor.lightBillboards, entity)

    Editor.updateSceneTree()
    Editor.select(light)
end

--- Adds a new sound to the current scene.
-- @param path The sound file to load.
function Editor.addSound(path)
    local sound = NeoLua.level:getCurrentScene():addNewLight()
    sound:setName(findName("Sound", false))
    sound:setPosition(Editor.getSelectionCenter())

    local entity = Editor.addObjectBillboard(light, "assets/editor/meshes/objects/sound.dae")
    table.insert(Editor.soundBillboards, entity)

    Editor.updateSceneTree()
    Editor.select(sound)
end

--- Adds a new entity mesh to the current scene.
-- @param path The mesh file to load.
function Editor.addEntity(path)
    local mesh = NeoLua.level:loadMesh(path)
    local entity = NeoLua.level:getCurrentScene():addNewEntity(mesh)
    entity:setName(findName("Entity", false))
    entity:setPosition(Editor.getSelectionCenter())

    Editor.updateSceneTree()
    Editor.select(entity)
end

--- Adds a new text to the current scene.
-- @param path The font file to load.
function Editor.addText(path)
    local font = NeoLua.level:loadFont(path, 36)
    local entity = NeoLua.level:getCurrentScene():addNewText(font)
    entity:setName(findName("Text", false))
    entity:setPosition(Editor.getSelectionCenter())
    entity:setText("Text")

    Editor.updateSceneTree()
    Editor.select(entity)
end

Editor.mx = 0
Editor.my = 0

Editor.requestLevelLoad = nil

Editor.loadInputSystem()
Editor.inputMethod = dofile(Editor.inputMethods["MaratisInput.lua"]) or function() end

Editor.lastPoint = NeoLua.Vector3()
Editor.currentSelection = {}

-- Load translations
Editor.loadTranslationList()

Editor.setupLevel()

--infoLog("Camera: " .. Editor.sceneCamera:getFov())
--print(Editor.sceneCamera:getPosition().x, Editor.sceneCamera:getPosition().y, Editor.sceneCamera:getPosition().z)

Editor.loadMeshes()

Editor.setupTranslatedUI("english.csv")
--Editor.setupTranslatedUI("german.csv")
Editor.loadUI()

NeoLua.system:setWindowTitle("Neo Scene Editor")
NeoLua.system:showCursor()

Editor.project = { directory = NeoLua.system:getWorkingDirectory(), level = ""}
infoLog("Loaded editor from project: " .. Editor.project.directory)

infoLog(os.tmpname())
