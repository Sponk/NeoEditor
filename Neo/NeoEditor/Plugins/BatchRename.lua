-- This plugin allows you to rename your whole selection at once.

require("plugins.libs.libplugin")

setPluginName("Batch Object Rename")
setPluginAuthor("Yannick Pflanzer")
setPluginDescription("A plugin that renames all selected objects.")

addEditorMenu("Rename Selection", "rename_callback")

window = Window(0,0,280,100, "Rename")
input = Input(60,10,200,30, "Name:")
okButton = Button(60,50,200,30, "Rename Objects")

okButton:setCallback("rename_button_callback")

window:add(input)
window:add(okButton)

function rename_callback()
   window:show()
end

function rename_button_callback()

   local selection = getCurrentSelection()
   local objNumber = 0
   local newName = input:getValue()
   
   for i = 1, #selection, 1 do

	  while objectExists(newName .. objNumber) do
		 objNumber = objNumber + 1
	  end

	  debugLog("Renaming object: " .. getName(selection[i]) .. " to " .. newName..objNumber)
	  renameObject(selection[i], newName .. objNumber)
   end   
end
