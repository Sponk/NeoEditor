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
        name = "physicsEnabledCheckButton",
        type = "CheckButton",
        label = tr("Physically Active"),
        x = 10,
        y = 170,
        size = 15,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [16] = {
        name = "massLabel",
        type = "Label",
        x = 10,
        y = 200,
        w = 0,
        h = 0,
        label = tr("Mass:")
      },
      
      [17] = {
        name = "massInput",
        type = "InputField",
        x = 10,
        y = 210,
        w = 280,
        h = 20,
	inputType = NeoLua.DECIMAL_INPUT,
        label = "0",
        callback = "updateEntityConfig"
      },
      
      [18] = {
        name = "ghostEnabledCheckButton",
        type = "CheckButton",
        label = tr("Ghost"),
        x = 10,
        y = 240,
        size = 15,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [19] = {
        name = "frictionLabel",
        type = "Label",
        x = 10,
        y = 270,
        w = 0,
        h = 0,
        label = tr("Friction:")
      },
      
      [20] = {
        name = "frictionInput",
        type = "InputField",
        x = 10,
        y = 280,
        w = 280,
        h = 20,
	inputType = NeoLua.DECIMAL_INPUT,
        label = "0",
        callback = "updateEntityConfig"
      },
      
      [21] = {
        name = "restitutionLabel",
        type = "Label",
        x = 10,
        y = 310,
        w = 0,
        h = 0,
        label = tr("Restitution:")
      },
      
      [22] = {
        name = "restitutionInput",
        type = "InputField",
        x = 10,
        y = 320,
        w = 280,
        h = 20,
	inputType = NeoLua.DECIMAL_INPUT,
        label = "0",
        callback = "updateEntityConfig"
      },
      
       [23] = {
        name = "linearDampingLabel",
        type = "Label",
        x = 10,
        y = 350,
        w = 0,
        h = 0,
        label = tr("Linear Damping:")
      },
      
      [24] = {
        name = "linearDampingInput",
        type = "InputField",
        x = 10,
        y = 360,
        w = 280,
        h = 20,
	inputType = NeoLua.DECIMAL_INPUT,
        label = "0",
        callback = "updateEntityConfig"
      },
      
      [25] = {
        name = "angularDampingLabel",
        type = "Label",
        x = 10,
        y = 390,
        w = 0,
        h = 0,
        label = tr("Angular Damping:")
      },
      
      [26] = {
        name = "angularDampingInput",
        type = "InputField",
        x = 10,
        y = 400,
        w = 280,
        h = 20,
	inputType = NeoLua.DECIMAL_INPUT,
        label = "0",
        callback = "updateEntityConfig"
      },
      
      [27] = {
        name = "angularFactorLabel",
        type = "Label",
        x = 10,
        y = 430,
        w = 0,
        h = 0,
        label = tr("Angular Factor:")
      },
      
      [28] = {
        name = "angularFactorInput",
        type = "InputField",
        x = 10,
        y = 440,
        w = 280,
        h = 20,
	inputType = NeoLua.DECIMAL_INPUT,
        label = "1",
        callback = "updateEntityConfig"
      },
      
      [29] = {
        name = "linearFactorLabel",
        type = "Label",
        x = 10,
        y = 470,
        w = 0,
        h = 0,
        label = tr("Linear Factor:")
      },
      
      [30] = {
        name = "lfxInput",
        type = "InputField",
        x = 10,
        y = 480,
        w = 60,
        h = 20,
        label = "1",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [31] = {
        name = "lfyInput",
        type = "InputField",
        x = 85,
        y = 480,
        w = 60,
        h = 20,
        label = "1",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [32] = {
        name = "lfzInput",
        type = "InputField",
        x = 160,
        y = 480,
        w = 60,
        h = 20,
        label = "1",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      {
        name = "physicsConstraintsButton",
        type = "Button",
        x = 10,
        y = 530,
        w = 280,
        h = 20,
        label = tr("Physics Constraints")--,
        --callback = ""      
      }
    }
  }
}

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
  
  local phys = entityEditor.entity:getPhysicsProperties()
  dlg["physicsEnabledCheckButton"]:setValue(phys ~= nil)
  
  if phys ~= nil then
	dlg["massInput"]:setLabel(round(phys:getMass(), 3))
	dlg["ghostEnabledCheckButton"]:setValue(phys:isGhost())
	dlg["frictionInput"]:setLabel(round(phys:getFriction(), 3))
	dlg["restitutionInput"]:setLabel(round(phys:getRestitution(), 3))
	dlg["linearDampingInput"]:setLabel(round(phys:getLinearDamping(), 3))
	dlg["angularDampingInput"]:setLabel(round(phys:getAngularDamping(), 3))
	dlg["angularFactorInput"]:setLabel(round(phys:getAngularFactor(), 3))
	
	local linearFactor = phys:getLinearFactor()
	dlg["lfxInput"]:setLabel(round(linearFactor.x, 3))
	dlg["lfyInput"]:setLabel(round(linearFactor.y, 3))
	dlg["lfzInput"]:setLabel(round(linearFactor.z, 3))
  else
	dlg["massInput"]:setLabel("0")
	dlg["ghostEnabledCheckButton"]:setValue(false)
	dlg["frictionInput"]:setLabel(0)
	dlg["restitutionInput"]:setLabel(0)
	dlg["linearDampingInput"]:setLabel(0)
	dlg["angularDampingInput"]:setLabel(0)
	
	dlg["angularFactorInput"]:setLabel(1)
	dlg["lfxInput"]:setLabel(1)
	dlg["lfyInput"]:setLabel(1)
	dlg["lfzInput"]:setLabel(1)
  end
  
  entityEditor.entity:enableWireframe(true)
  
  dlg.window:setVisible(true)
  Gui.wm:selectWindow(dlg.window)
  dlg.window:update()
end

function dlg.create()
	entityEditor.dlg = Gui.loadFromTable(dlg)
	entityEditor.dlg["window"].window:setVisible(false)
	return entityEditor
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
  
  if dlg["physicsEnabledCheckButton"]:getValue() 
	and entityEditor.entity:getPhysicsProperties() == nil then
	
	entityEditor.entity:createPhysicsProperties()
	
  elseif not dlg["physicsEnabledCheckButton"]:getValue() 
	and entityEditor.entity:getPhysicsProperties() ~= nil then
		
	entityEditor.entity:deletePhysicsProperties()
  end
  
  local phys = entityEditor.entity:getPhysicsProperties() 
  if phys ~= nil then
	phys:setMass(dlg["massInput"]:getLabel())
	phys:setGhost(dlg["ghostEnabledCheckButton"]:getValue())
	
	phys:setFriction(dlg["frictionInput"]:getLabel())
	phys:setRestitution(dlg["restitutionInput"]:getLabel())
	phys:setLinearDamping(dlg["linearDampingInput"]:getLabel())
	phys:setAngularDamping(dlg["angularDampingInput"]:getLabel())
	phys:setAngularFactor(dlg["angularFactorInput"]:getLabel())
	
	phys:setLinearFactor(NeoLua.Vector3(dlg["lfxInput"]:getLabel(), 
				dlg["lfyInput"]:getLabel(), dlg["lfzInput"]:getLabel()))
  end
  
end

function entityEditor.updateData()
  if entityEditor.entity ~= nil then
    entityEditor.setShownObject(entityEditor.entity:getName())
  end
end

return dlg
