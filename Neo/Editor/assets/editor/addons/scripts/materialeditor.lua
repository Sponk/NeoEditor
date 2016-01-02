--- materialeditor.lua - Contains the material editor and its logic.

local addon = {
	name = "Material Editor",
	description = "Provides a dialog that allows changing the material properties of an entity.",
	author = "Yannick Pflanzer"
}

local privateData = {}

local function getTextureFromMaterial(material, passId)
	if material == nil or passId < 0 or passId >= material:getTexturesPassNumber() then
		infoLog("Could not find texture with ID " .. tostring(passId))
		return nil
	end

	local texpass = material:getTexturePass(passId)
	if texpass ~= nil then
		return texpass:getTexture()
	end
	return nil
end

function showMaterial(id)

	if id <= 0 or not privateData.entity or id > privateData.entity:getMaterialsNumber() then return end

	local material = privateData.entity:getMaterial(id-1)
	local dlg = privateData.dlg["window"]

	privateData.materialID = id
	privateData.material = material

	dlg["opacity"]:setLabel(round(material:getOpacity(), 3))
	dlg["opacitySlider"]:setValue(dlg["opacity"]:getLabel() * 100)

	dlg["shininess"]:setLabel(round(material:getShininess(), 3))
	dlg["shininessSlider"]:setValue(dlg["shininess"]:getLabel() * 100)

	local vector = material:getDiffuse()
	dlg["diffuseR"]:setLabel(round(vector.x, 3))
	dlg["diffuseG"]:setLabel(round(vector.y, 3))
	dlg["diffuseB"]:setLabel(round(vector.z, 3))

	vector = material:getSpecular()
	dlg["specularR"]:setLabel(round(vector.x, 3))
	dlg["specularG"]:setLabel(round(vector.y, 3))
	dlg["specularB"]:setLabel(round(vector.z, 3))

	vector = material:getEmit()
	dlg["emitR"]:setLabel(round(vector.x, 3))
	dlg["emitG"]:setLabel(round(vector.y, 3))
	dlg["emitB"]:setLabel(round(vector.z, 3))

	local list = privateData.dlg["window"]["texturescroll"]["list"]
	local numTextures = material:getTexturesPassNumber()

	list:clear()

	for i = 1, numTextures, 1 do
		local texture = getTextureFromMaterial(material, i - 1)
		local filename = texture:getTextureRef():getFilename()

		local index = filename:match("^.*()/")

		filename = filename:sub(index+1) or "<nofile>"
		list:addEntry(filename)
	end
end

function showTextureCallback()

end

function textureFileOpenCallback()
	local id = privateData.dlg["window"]["texturescroll"]["list"]:getSelectedEntry()

	if id == -1 then return end

	local filename = Editor.openFileDlg:getSelectedFilename()
	if filename == nil or filename == "" then return end

	local texture = getTextureFromMaterial(privateData.material, id)
	local ref = NeoLua.level:loadTexture(filename, true)

	if ref then
		texture:setTextureRef(ref)
	end

	showMaterial(privateData.materialID)
	Gui.wm:selectWindow(privateData.dlg["window"].window)
end

function changeTextureCallback()

	if Editor.openFileDlg:isVisible() then return end

	local id = privateData.dlg["window"]["texturescroll"]["list"]:getSelectedEntry()
	if id == -1 then return end

	local texref = getTextureFromMaterial(privateData.material, id):getTextureRef()
	local filename = texref:getFilename()
	local index = filename:match("^.*()/")
	filename = filename:sub(1, index - 1)

	infoLog("Selecting texture: " .. filename)
	Editor.openFileDlg:setScriptCallback(AddonSystem.registerCallback(addon.name, textureFileOpenCallback))
	Editor.openFileDlg:setVisible(true)
	Editor.openFileDlg:setFilter("(.*)|(.*\\.png)|(.*\\.jpg)|(.*\\.tga)|(.*\\.jpeg)")
	Editor.openFileDlg:readDirectory(filename)

	Gui.wm:selectWindow(Editor.openFileDlg)
end


function updateDiffuse()
	local dlg = privateData.dlg["window"]

	privateData.material:setDiffuse(NeoLua.Vector3(dlg["diffuseR"]:getLabel(),
		dlg["diffuseG"]:getLabel(), dlg["diffuseB"]:getLabel()))
end

function updateSpecular()
	local dlg = privateData.dlg["window"]

	privateData.material:setSpecular(NeoLua.Vector3(dlg["specularR"]:getLabel(),
		dlg["specularG"]:getLabel(), dlg["specularB"]:getLabel()))
end

function updateEmit()
	local dlg = privateData.dlg["window"]

	privateData.material:setEmit(NeoLua.Vector3(dlg["emitR"]:getLabel(),
		dlg["emitG"]:getLabel(), dlg["emitB"]:getLabel()))
end

function updateOpacity()
	local shininessInput = privateData.dlg["window"]["opacity"]
	privateData.material:setOpacity(shininessInput:getLabel())
	privateData.dlg["window"]["opacitySlider"]:setValue(shininessInput:getLabel())
end

function updateShininess()
	local shininessInput = privateData.dlg["window"]["shininess"]
	privateData.material:setShininess(shininessInput:getLabel())
	privateData.dlg["window"]["shininessSlider"]:setValue(shininessInput:getLabel())
end

function updateShininessSlider()
	local shininessInput = privateData.dlg["window"]["shininess"]

	shininessInput:setLabel(round(privateData.dlg["window"]["shininessSlider"]:getValue() / 100, 3))
	privateData.material:setShininess(shininessInput:getLabel())
end

function updateOpacitySlider()
	local opacityInput = privateData.dlg["window"]["opacity"]

	opacityInput:setLabel(round(privateData.dlg["window"]["opacitySlider"]:getValue() / 100, 3))
	privateData.material:setOpacity(opacityInput:getLabel())
end

function showMaterialCallback()
	local tree = privateData.dlg["window"]["scrollpanel"]["tree"]
	local treemodel = tree:getTreeModel()

	local selected = tree:getSelected()
	index = selected:match("^.*() ")

	showMaterial(tonumber(selected:sub(index+1)))
end

function genDlg()
	local offset = 45

	return {
	[1] = {
		name = "window",
		type = "Window",
		x = 500,
		y = 500,
		w = 460,
		h = 280,
		label = tr("Material Editor"),
		content = {
			{name = "button", type = "Button", x = 10, y = 250, w = 100, h = 20, label = tr("Close")},

			{name = "entitynameLabel", type = "Label", x = 10, y = 10, w = 0, h = 0, label = tr("Entity Name:") },
			{name = "nameLabel", type = "Label", x = 100, y = 10, w = 0, h = 0, label = "" },

			{
				name = "scrollpanel", type = "ScrollPane", x = 10, y = 25,
				w = 210, h = 20,
				content = {
					{name = "tree", type = "Tree", x = 0, y = 0,
						w = 200, h = 0, callback = AddonSystem.registerCallback(addon.name, showMaterialCallback)}
				}
			},

			{
				name = "texturescroll", type = "ScrollPane", x = 260, y = 25,
				w = 180, h = 100,
				content = {
					{name = "list", type = "List", x = 0, y = 0,
						w = 200, h = 0, callback = AddonSystem.registerCallback(addon.name, showTextureCallback)}
				}
			},

			{
				name = "changeTextureButton", type = "Button", x = 260, y = 135,
				w = 180, h = 20,
				label = tr("Change Texture"),
				callback = AddonSystem.registerCallback(addon.name, changeTextureCallback)
			},

			{name = "opacityLabel", type = "Label", x = 10, y = offset + 10, w = 0, h = 20, label = tr("Opacity:")},
			{name = "opacity", type = "InputField", x = 70, y = offset + 10, w = 50, h = 20, inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateOpacity)},
			{name = "opacitySlider", type = "Slider", x = 130, y = offset + 15, w = 85, h = 1, range = {0,100}, callback = AddonSystem.registerCallback(addon.name, updateOpacitySlider)},

			{name = "shininessLabel", type = "Label", x = 10, y = offset + 40, w = 0, h = 20, label = tr("Shininess:")},
			{name = "shininess", type = "InputField", x = 70, y = offset + 40, w = 50, h = 20, inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateShininess)},
			{name = "shininessSlider", type = "Slider", x = 130, y = offset + 45, w = 85, h = 1, range = {0,400}, callback = AddonSystem.registerCallback(addon.name, updateShininessSlider)},

			{ name = "diffuseLabel", type = "Label", x = 10, y = offset + 80, w = 0, h = 0, label = tr("Diffuse:") },
			{ name = "diffuseR", type = "InputField", x = 10, y = offset + 90, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT,
				callback = AddonSystem.registerCallback(addon.name, updateDiffuse)},

			{ name = "diffuseG", type = "InputField", x = 85, y = offset + 90, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateDiffuse) },
			{ name = "diffuseB", type = "InputField", x = 160, y = offset + 90, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateDiffuse) },

			{ name = "specularLabel", type = "Label", x = 10, y = offset + 120, w = 0, h = 0, label = tr("Specular:") },
			{ name = "specularR", type = "InputField", x = 10, y = offset + 130, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateSpecular) },
			{ name = "specularG", type = "InputField", x = 85, y = offset + 130, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateSpecular) },
			{ name = "specularB", type = "InputField", x = 160, y = offset + 130, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateSpecular) },

			{ name = "emitLabel", type = "Label", x = 10, y = offset + 160, w = 0, h = 0, label = tr("Emit:") },
			{ name = "emitR", type = "InputField", x = 10, y = offset + 170, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateEmit) },
			{ name = "emitG", type = "InputField", x = 85, y = offset + 170, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateEmit) },
			{ name = "emitB", type = "InputField", x = 160, y = offset + 170, w = 60, h = 20, label = "", inputType = NeoLua.DECIMAL_INPUT, callback = AddonSystem.registerCallback(addon.name, updateEmit) }
		}
	}
}
end

local function showDialog()

	-- If the current selection is not empty and is something that has
	-- materials, load the data and proceed. Else, return.
	if not Editor.currentSelection[1]
			or not Editor.currentSelection[1].getMaterialsNumber then
		return
	end

	privateData.entity = Editor.currentSelection[1]

	-- Set up data
	showMaterial(1)

	local tree = privateData.dlg["window"]["scrollpanel"]["tree"]
	local treemodel = tree:getTreeModel()
	local numMat = privateData.entity:getMaterialsNumber()

	tree:clear()
	for i = 1, numMat, 1 do
		treemodel:addChild("Material " .. i)
	end

	privateData.dlg["window"]["nameLabel"]:setLabel(privateData.entity:getName())

	-- Set up window
	local system = NeoLua.NeoEngine.getInstance():getSystemContext()
	local res = system:getScreenSize()
	local size = privateData.win:getSize()

	privateData.win:setPosition(NeoLua.Vector2(res.x/2 - size.x/2, res.y/2 - size.y/2))
	privateData.win:setVisible(true)
	Gui.wm:selectWindow(privateData.win)
end

function hideDialog()
	privateData.win:setVisible(false)
end

function addon.onBegin()
	privateData.dlg = Gui.loadFromTable(genDlg())
	privateData.win = privateData.dlg["window"].window
	privateData.win:setVisible(false)

	Shortcuts.addShortcut({"LCONTROL", "M"}, showDialog)
	Shortcuts.addShortcut({"RCONTROL", "M"}, showDialog)

	local hideCb = AddonSystem.registerCallback(addon.name, hideDialog)

	privateData.dlg["window"]["button"]:setScriptCallback(hideCb)
end

function addon.onEnd()

end

return addon