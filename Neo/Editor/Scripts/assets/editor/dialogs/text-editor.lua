--- text-editor.lua - Contains the text editor dialog

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
    label = tr("Edit Text"),
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
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
	callback = "updateTextConfig"
      },

      [15] = {
	name = "textLabel",
	type = "Label",
	x = 10,
	y = 180,
	w = 0,
	h = 0,
	label = tr("Text:")
      },

      [16] = {
	name = "textInput",
	type = "InputField",
	x = 10,
	y = 190,
	w = 280,
	h = 100,
	label = "",
	callback = "updateTextConfig"
      },

      [17] = {
        name = "colorLabel",
        type = "Label",
        x = 10,
        y = 310,
        w = 0,
        h = 0,
        label = tr("Color:")
      },

      [18] = {
        name = "crInput",
        type = "InputField",
        x = 10,
        y = 320,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateTextConfig"
      },

      [19] = {
        name = "cgInput",
        type = "InputField",
        x = 80,
        y = 320,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateTextConfig"
      },

      [20] = {
        name = "cbInput",
        type = "InputField",
        x = 150,
        y = 320,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateTextConfig"
      },
      [21] = {
        name = "caInput",
        type = "InputField",
        x = 220,
        y = 320,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateTextConfig"
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
     editor.entity = NeoLua.level:getCurrentScene():getTextByName(objName)
  end

  -- Ignore if the selected entry is no entity  
  if editor.entity == nil or editor.entity:getType() ~= NeoLua.OBJECT3D_TEXT then
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

  local color = editor.entity:getColor()
  dlg["crInput"]:setLabel(round(color.x, 4))
  dlg["cgInput"]:setLabel(round(color.y, 4))
  dlg["cbInput"]:setLabel(round(color.z, 4))
  dlg["caInput"]:setLabel(round(color.w, 4))

  dlg["textInput"]:setLabel(editor.entity:getText())

  dlg.window:setVisible(true)
  Gui.wm:selectWindow(dlg.window)
  dlg.window:update()
end

function dlg.create()
	editor.dlg = Gui.loadFromTable(dlg)
	editor.dlg["window"].window:setVisible(false)
	return editor
end

function updateTextConfig()
  -- infoLog("Updating entity!")
  local dlg = editor.dlg["window"]
  editor.entity:setPosition(NeoLua.Vector3(dlg["pxInput"]:getLabel(),
                                  dlg["pyInput"]:getLabel(), dlg["pzInput"]:getLabel()))
  
  editor.entity:setEulerRotation(NeoLua.Vector3(dlg["rxInput"]:getLabel(),
                                  dlg["ryInput"]:getLabel(), dlg["rzInput"]:getLabel()))
                                  
  editor.entity:setScale(NeoLua.Vector3(dlg["sxInput"]:getLabel(),
                                  dlg["syInput"]:getLabel(), dlg["szInput"]:getLabel()))

  editor.entity:setColor(NeoLua.Vector4(dlg["crInput"]:getLabel(), dlg["cgInput"]:getLabel(), dlg["cbInput"]:getLabel(), dlg["caInput"]:getLabel()))

  editor.entity:setText(dlg["textInput"]:getLabel())

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
