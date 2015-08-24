--- entity-editor.lua - Contains the entity editor dialog

local res = NeoLua.system:getScreenSize()
local entityEditor = {}

local dlg = {
  [1] = {
    name = "window",
    type = "Window",
    x = res.x - 310,
    y = 70,
    w = 300,
    h = 650,
    label = tr("Edit Entity"),
    content = {
      [1] = {
        name = "nameLabel",
        type = "Label",
        x = 10,
        y = 10,
        w = 0,
        h = 0,
        label = tr("Name:")
      },
      
      [2] = {
        name = "nameInput",
        type = "InputField",
        x = 10,
        y = 20,
        w = 280,
        h = 20,
        label = "",
        callback = "updateEntityConfig"
      },
      
      [3] = {
        name = "positionLabel",
        type = "Label",
        x = 10,
        y = 50,
        w = 0,
        h = 0,
        label = tr("Position:")
      },
      
      [4] = {
        name = "pxInput",
        type = "InputField",
        x = 10,
        y = 60,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [5] = {
        name = "pyInput",
        type = "InputField",
        x = 85,
        y = 60,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [6] = {
        name = "pzInput",
        type = "InputField",
        x = 160,
        y = 60,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      
      [7] = {
        name = "rotationLabel",
        type = "Label",
        x = 10,
        y = 90,
        w = 0,
        h = 0,
        label = tr("Rotation:")
      },
      
      [8] = {
        name = "rxInput",
        type = "InputField",
        x = 10,
        y = 100,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [9] = {
        name = "ryInput",
        type = "InputField",
        x = 85,
        y = 100,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [10] = {
        name = "rzInput",
        type = "InputField",
        x = 160,
        y = 100,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      
      [11] = {
        name = "scaleLabel",
        type = "Label",
        x = 10,
        y = 130,
        w = 0,
        h = 0,
        label = tr("Scale:")
      },
      
      [12] = {
        name = "sxInput",
        type = "InputField",
        x = 10,
        y = 140,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [13] = {
        name = "syInput",
        type = "InputField",
        x = 85,
        y = 140,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [14] = {
        name = "szInput",
        type = "InputField",
        x = 160,
        y = 140,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [15] = {
        name = "physicsPropertiesButton",
        type = "Button",
        x = 10,
        y = 185,
        w = 280,
        h = 20,
        label = tr("Physics Properties")--,
        --callback = ""      
      }
    }
  }
}

entityEditor.dlg = Gui.loadFromTable(dlg)
entityEditor.dlg["window"].window:setVisible(false)

function entityEditor.setShownObject(objName)

  if objName == nil then
	entityEditor.entity = nil
	return
  end

  if entityEditor.entity ~= nil then
    entityEditor.entity:enableWireframe(false)
  end

  entityEditor.entity = NeoLua.level:getCurrentScene():getEntityByName(objName)

  -- Ignore if the selected entry is no entity  
  if entityEditor.entity == nil then
    entityEditor.dlg["window"].window:setVisible(false)
    return
  end
  
  local dlg = entityEditor.dlg["window"]
  dlg["nameInput"]:setLabel(objName)
  
  local pos = entityEditor.entity:getPosition()
  dlg["pxInput"]:setLabel(round(pos.x, 3))
  dlg["pyInput"]:setLabel(round(pos.y, 3))
  dlg["pzInput"]:setLabel(round(pos.z, 3))
  
  local rot = entityEditor.entity:getEulerRotation()
  dlg["rxInput"]:setLabel(round(rot.x, 3))
  dlg["ryInput"]:setLabel(round(rot.y, 3))
  dlg["rzInput"]:setLabel(round(rot.z, 3))
  
  local scale = entityEditor.entity:getScale()
  dlg["sxInput"]:setLabel(round(scale.x, 3))
  dlg["syInput"]:setLabel(round(scale.y, 3))
  dlg["szInput"]:setLabel(round(scale.z, 3))
  
  entityEditor.entity:enableWireframe(true)
    
  dlg.window:setVisible(true)
  Gui.wm:selectWindow(dlg.window)
  dlg.window:update()
end

function updateEntityConfig()
  -- infoLog("Updating entity!")
  local dlg = entityEditor.dlg["window"]
  entityEditor.entity:setPosition(NeoLua.Vector3(dlg["pxInput"]:getLabel(), 
                                  dlg["pyInput"]:getLabel(), dlg["pzInput"]:getLabel()))
  
  entityEditor.entity:setEulerRotation(NeoLua.Vector3(dlg["rxInput"]:getLabel(), 
                                  dlg["ryInput"]:getLabel(), dlg["rzInput"]:getLabel()))
                                  
  entityEditor.entity:setScale(NeoLua.Vector3(dlg["sxInput"]:getLabel(), 
                                  dlg["syInput"]:getLabel(), dlg["szInput"]:getLabel()))
  
  -- Set new rounded values (no rotation > 360°)
  local rot = entityEditor.entity:getEulerRotation()
  dlg["rxInput"]:setLabel(round(rot.x, 3))
  dlg["ryInput"]:setLabel(round(rot.y, 3))
  dlg["rzInput"]:setLabel(round(rot.z, 3))
  
  local newName = dlg["nameInput"]:getLabel()
  if newName:len() > 0 and newName ~= entityEditor.entity:getName() then  
    newName = findName(newName)
    entityEditor.entity:setName(newName)
    Editor.updateSceneTree()
  end
  
  if newName:len() <= 0 then newName = entityEditor.entity:getName() end
    
  dlg["nameInput"]:setLabel(newName)
end

function entityEditor.updateData()
  if entityEditor.entity ~= nil then
    entityEditor.setShownObject(entityEditor.entity:getName())
  end
end

return entityEditor
