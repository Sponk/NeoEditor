--- main.lua - The editor main script
Editor = {}

local lfs = require("lfs")

function Editor.loadUI()
   Editor.sceneDlg = dofile("dialogs/scene-tree-desc.lua")
   Editor.updateSceneTree()

   local menubarTable = dofile("menubar.lua")
   Gui.loadFromTable(menubarTable)

   -- This one sets itself up
   Editor.aboutDlg = dofile("dialogs/aboutdlg.lua")
   -- Editor.aboutDlg = Gui.loadFromTable(aboutDlgTable)
   
   Editor.entityEditor = dofile("dialogs/entity-editor.lua")  
   Gui.wm:selectWindow(Editor.sceneDlg["window"].window)
end

local function addChildrenToTree(obj, node)
   local sz = obj:getChildrenNumber()

   for i = 0, sz-1, 1 do
	  local child = obj:getChild(i)
	  local childNode = node:addChild(child:getName())
	  addChildrenToTree(child, childNode)
   end
end

function Editor.updateSceneTree()
   local scene = NeoLua.level:getCurrentScene()
   local sz = scene:getObjectsNumber()
   
   local tree = Editor.sceneDlg["window"]["layout"]["scrollpanel"]["tree"]
   tree:clear()

   local node = tree:getTreeModel()
   
   for i = 0, sz-1, 1 do
	  local object = scene:getObjectByIndex(i)
	  
	  if not object:hasParent() and object:getId() ~= Editor.sceneCamera:getId() then
	   if object:getName():len() > 0 then
		  local childNode = node:addChild(object:getName())
		  addChildrenToTree(object, childNode)
		 end
	  end
   end
end

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

function Editor.loadMeshes()
  Editor.sceneMeshes = {}
  
  NeoLua.arrow = NeoLua.level:loadMesh("assets/editor/meshes/arrow.obj")
  local scene = Editor.overlayScene
  local z = scene:addNewEntity(NeoLua.arrow)
  local x = scene:addNewEntity(NeoLua.arrow)
  local y = scene:addNewEntity(NeoLua.arrow)
  
  x:setEulerRotation(NeoLua.Vector3(0,90,0))
  y:setEulerRotation(NeoLua.Vector3(90,0,0))
  
  Editor.sceneMeshes = {y = y, x = x, z = z}
  
end

local function castRay(entity, rayO, rayD)

      entity:getMesh():updateBoundingBox()
      local box = entity:getMesh():getBoundingBox()

      local pos = entity:getTransformedPosition()
      local iMatrix = entity:getMatrix():getInverse()
      
      local localRayO = iMatrix * rayO
      local localRayD = iMatrix * rayD
      if not NeoLua.isEdgeToBoxCollision(localRayO, localRayD, box.min, box.max) then
        return unpack({false, nil})
      end

      local mesh = entity:getMesh()
      local sz = mesh:getSubMeshsNumber()    
      
      for i = 0, sz - 1, 1 do
        local submesh = mesh:getSubMesh(i)
        local isz = submesh:getIndicesSize()
        for j = 0, isz - 1, 3 do
          local v1 = submesh:getVertex(submesh:getIndex(j))
          local v2 = submesh:getVertex(submesh:getIndex(j+1))
          local v3 = submesh:getVertex(submesh:getIndex(j+2))
          
          --print("v1: ", v1.x, v1.y, v1.z)
          --print("v2: ", v2.x, v2.y, v2.z)
          --print("v3: ", v3.x, v3.y, v3.z)
          
          local ipoint = NeoLua.Vector3()
          
          -- bool isEdgeTriangleIntersection(const Vector3 & origin, const Vector3 & dest, const Vector3 & a, const Vector3 & b, const Vector3 & c, const Vector3 & normal, Vector3 * point)
          local retval = NeoLua.isEdgeTriangleIntersection(localRayO, localRayD, v1, v2, v3, NeoLua.getTriangleNormal(v1, v2, v3), ipoint)
          
          if retval then
            --print("ipoint: ", ipoint.x, ipoint.y, ipoint.z)
            return unpack({true, entity:getMatrix() * ipoint})
          end
        end
      end
      
      return unpack({false, nil})
end

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

  if NeoLua.input:isKeyPressed("MOUSE_BUTTON_LEFT") and Editor.selectedArrow ~= nil then
    
    value,point = castRay(Editor.selectedArrow, rayO, rayD)
    if not value then return end
        
    --Editor.selectedArrow:translate(Editor.lastPoint - point)   
  elseif NeoLua.input:isKeyPressed("MOUSE_BUTTON_LEFT") and #Editor.currentSelection == 1 then
    
    value,point = castRay(Editor.currentSelection[1], rayO, rayD)
    --if not value then return end
    
    --if value then
    
      local p1 = camera:getUnProjectedPoint(NeoLua.Vector3(mx, my, 0.3))
      local p2 = camera:getUnProjectedPoint(NeoLua.Vector3(Editor.mx*res.x, (1-Editor.my)*res.y, 0.3))
      local dif = p1 - p2
      
      -- print("mx: ", mx, my)
      -- print("omx: ", Editor.mx, Editor.my)
      -- print("p1: ", p1.x, p1.y, p1.z)
      -- print("p2: ", p2.x, p2.y, p2.z)
      -- print("dif: ", dif.x, dif.y, dif.z)
      
      local scale = (camera:getPosition() - Editor.currentSelection[1]:getTransformedPosition()):getLength()
      
      Editor.currentSelection[1]:translate(dif*scale*0.7)
      Editor.currentSelection[1]:updateMatrix()
      
      Editor.entityEditor.updateData()
      
      Editor.lastPoint = point
    --end
  end

  Editor.selectedArrow = nil

  if NeoLua.input:onKeyDown("MOUSE_BUTTON_LEFT") then 
    -- Check arrows
    if #Editor.currentSelection > 0 then
      local meshes = Editor.sceneMeshes
      if castRay(meshes.x, rayO, rayD) then
        Editor.selectedArrow = meshes.x
        return
      elseif castRay(meshes.y, rayO, rayD) then
        Editor.selectedArrow = meshes.y
        return
      elseif castRay(meshes.z, rayO, rayD) then
        Editor.selectedArrow = meshes.z
        return
      end
    end
 
    local possibleSelection = {}
    for i = 0, numObjects - 1, 1 do
      local entity = scene:getEntityByIndex(i)      
      if castRay(entity, rayO, rayD) then
        
        table.insert(possibleSelection, entity)
        --Editor.entityEditor.setShownObject(entity:getName())
        
        --return entity
      end
    end
    
    if #possibleSelection <= 0 then Editor.select(nil) return end
    
    local camPos = camera:getPosition()
    local selectedEntity = possibleSelection[1]
    local lastDistance = (camPos-selectedEntity:getTransformedPosition()):getLength()
    for k,v in ipairs(possibleSelection) do
      local dst = (camPos-v:getTransformedPosition()):getLength()
      if dst < lastDistance then
        lastDistance = dst
        selectedEntity = v
      end
    end
    
    Editor.select(selectedEntity)
  end
end

function Editor.select(obj)

    local arrows = Editor.sceneMeshes

    if obj == nil then
    
      for k,v in ipairs(Editor.currentSelection) do
        v:enableWireframe(false)
      end

      arrows.x:setActive(false)
      arrows.y:setActive(false)
      arrows.z:setActive(false)
      
      Editor.currentSelection = {}
      return
    end

    local position = obj:getTransformedPosition()
    
    arrows.x:setActive(true)
    arrows.y:setActive(true)
    arrows.z:setActive(true)
    
    arrows.x:setPosition(position)
    arrows.y:setPosition(position)
    arrows.z:setPosition(position)

    Editor.entityEditor.setShownObject(obj:getName())
    Editor.currentSelection = {obj}
    
    Editor.sceneDlg["window"]["layout"]["scrollpanel"]["tree"]:selectEntry(obj:getName())
end

function quitCallback()
  NeoLua.engine:setActive(false)
end

function update(dt)

  -- Calculate mouse delta
  local mx = NeoLua.input:getAxis("MOUSE_X")
  local my = NeoLua.input:getAxis("MOUSE_Y")
  
  Editor.dx = mx - Editor.mx
  Editor.dy = my - Editor.my

  if not NeoLua.Neo2DEngine.getInstance():isMouseOnGui() then
    Editor.inputMethod()
    Editor.selectObject()
  end
  
  Editor.mx = mx
  Editor.my = my
end

Editor.loadTranslationList()
Translator.swapTranslation("english.csv")

Editor.mx = 0
Editor.my = 0

Editor.loadInputSystem()
Editor.inputMethod = dofile(Editor.inputMethods["MaratisInput.lua"]) or function() end
Editor.sceneCamera = NeoLua.level:getCurrentScene():addNewCamera()
Editor.overlayScene = NeoLua.level:addNewScene()
Editor.overlayScene:setName("EditorOverlay")

Editor.overlaySceneId = NeoLua.level:getScenesNumber()
-- Editor.sceneCamera:setSceneLayer(Editor.overlaySceneId)

Editor.sceneCamera:setName("MainSceneCamera")
Editor.sceneCamera:setPosition(NeoLua.Vector3(0, -200, 200))
Editor.sceneCamera:setEulerRotation(NeoLua.Vector3(90,0,0))
Editor.sceneCamera:setClippingFar(100000)
Editor.sceneCamera:setEulerRotation(NeoLua.Vector3(40, 0, 0))

Editor.overlayCamera = Editor.overlayScene:addNewCamera(Editor.sceneCamera)
Editor.overlayCamera:setName("Camera")

Editor.lastPoint = NeoLua.Vector3()
Editor.currentSelection = {}

NeoLua.level:getCurrentScene():setCurrentCamera(Editor.sceneCamera)

--infoLog("Camera: " .. Editor.sceneCamera:getFov())
--print(Editor.sceneCamera:getPosition().x, Editor.sceneCamera:getPosition().y, Editor.sceneCamera:getPosition().z)

Editor.loadMeshes()
Editor.loadUI()
NeoLua.system:setWindowTitle("Neo Scene Editor")
infoLog("Loaded editor!")
