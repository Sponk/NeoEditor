--- light-editor.lua - Contains the light editor dialog

local res = NeoLua.system:getScreenSize()
local editor = {}

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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
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
	callback = "updateLightConfig"
      },

      [15] = {
	name = "radiusLabel",
	type = "Label",
	x = 10,
	y = 190,
	w = 0,
	h = 0,
	label = tr("Radius:")
      },

      [16] = {
	name = "radiusInput",
	type = "InputField",
	x = 10,
	y = 200,
	w = 280,
	h = 20,
	label = "",
	inputType = NeoLua.DECIMAL_INPUT,
	callback = "updateLightConfig"
      },

      [17] = {
	name = "intensityLabel",
	type = "Label",
	x = 10,
	y = 230,
	w = 0,
	h = 0,
	label = tr("Intensity:")
      },

      [18] = {
	name = "intensityInput",
	type = "InputField",
	x = 10,
	y = 240,
	w = 280,
	h = 20,
	label = "",
	inputType = NeoLua.DECIMAL_INPUT,
	callback = "updateLightConfig"
      }
    }
  }
}

function editor.setShownObject(objName)

  if objName == nil then
	editor.entity = nil
	return
  end

  if editor.entity == nil or editor.entity:getName() ~= objName then
     editor.entity = NeoLua.level:getCurrentScene():getLightByName(objName)
  end

  -- Ignore if the selected entry is no entity  
  if editor.entity == nil or editor.entity:getType() ~= NeoLua.OBJECT3D_LIGHT then
    editor.dlg["window"].window:setVisible(false)
    return
  end
  
  local dlg = editor.dlg["window"]
  dlg["nameInput"]:setLabel(objName)
  
  local pos = editor.entity:getPosition()
  dlg["pxInput"]:setLabel(round(pos.x, 3))
  dlg["pyInput"]:setLabel(round(pos.y, 3))
  dlg["pzInput"]:setLabel(round(pos.z, 3))
  
  local rot = editor.entity:getEulerRotation()
  dlg["rxInput"]:setLabel(round(rot.x, 3))
  dlg["ryInput"]:setLabel(round(rot.y, 3))
  dlg["rzInput"]:setLabel(round(rot.z, 3))
  
  local scale = editor.entity:getScale()
  dlg["sxInput"]:setLabel(round(scale.x, 3))
  dlg["syInput"]:setLabel(round(scale.y, 3))
  dlg["szInput"]:setLabel(round(scale.z, 3))

  dlg["radiusInput"]:setLabel(editor.entity:getRadius())
  dlg["intensityInput"]:setLabel(editor.entity:getIntensity())

  dlg.window:setVisible(true)
  Gui.wm:selectWindow(dlg.window)
  dlg.window:update()
end

function dlg.create()
	editor.dlg = Gui.loadFromTable(dlg)
	editor.dlg["window"].window:setVisible(false)
	return editor
end

function updateLightConfig()
  -- infoLog("Updating entity!")
  local dlg = editor.dlg["window"]
  editor.entity:setPosition(NeoLua.Vector3(dlg["pxInput"]:getLabel(),
                                  dlg["pyInput"]:getLabel(), dlg["pzInput"]:getLabel()))
  
  editor.entity:setEulerRotation(NeoLua.Vector3(dlg["rxInput"]:getLabel(),
                                  dlg["ryInput"]:getLabel(), dlg["rzInput"]:getLabel()))
                                  
  editor.entity:setScale(NeoLua.Vector3(dlg["sxInput"]:getLabel(),
                                  dlg["syInput"]:getLabel(), dlg["szInput"]:getLabel()))

  editor.entity:setRadius(dlg["radiusInput"]:getLabel())
  editor.entity:setIntensity(dlg["intensityInput"]:getLabel())

  -- Set new rounded values (no rotation > 360°)
  local rot = editor.entity:getEulerRotation()
  dlg["rxInput"]:setLabel(round(rot.x, 3))
  dlg["ryInput"]:setLabel(round(rot.y, 3))
  dlg["rzInput"]:setLabel(round(rot.z, 3))
  
  local newName = dlg["nameInput"]:getLabel()
  if newName:len() > 0 and newName ~= editor.entity:getName() then
    newName = findName(newName)
    editor.entity:setName(newName)
    Editor.updateSceneTree()
  end
  
  if newName:len() <= 0 then newName = editor.entity:getName() end
    
  dlg["nameInput"]:setLabel(newName)
end

function editor.updateData()
  if editor.entity ~= nil then
    editor.setShownObject(editor.entity:getName())
  end
end

return dlg
