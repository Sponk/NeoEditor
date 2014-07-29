setPluginName("Prefabs")
setPluginAuthor("Yannick Pflanzer")
setPluginDescription("A plugin that allow you to load and save your object selection as a prefab.")

addEditorMenu("Load Prefab", "load_callback")
addEditorMenu("Save selection as Prefab", "save_callback")

function load_callback()

end

function objectToXml(object)
   output = "<object name=\"" .. getName(object) .. "\"></object>"
   return output
end

function save_callback()
    selection = getCurrentSelection();
    
    if selection == nil then
        print("Selection is NIL!")
        return
    end
    
    if #selection == 0 then
       messagebox("No objects selected! Can't create prefab!") 
    end
    
    for i = 1, #selection, 1 do
       print(objectToXml(selection[i]))
    end
end