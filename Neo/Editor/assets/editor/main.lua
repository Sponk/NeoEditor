--- main.lua - The editor main script
Editor = {}

local lfs = require("lfs")

function Editor.loadUI()
   local sceneDlgTable = dofile("dialogs/scene-tree-desc.lua")
   Editor.sceneDlg = Gui.loadFromTable(sceneDlgTable)
   Editor.updateSceneTree()

   local menubarTable = dofile("menubar.lua")
   Gui.loadFromTable(menubarTable)

   -- This one sets itself up
   Editor.aboutDlg = dofile("dialogs/aboutdlg.lua")
   -- Editor.aboutDlg = Gui.loadFromTable(aboutDlgTable)
   
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
	  
	  if not object:hasParent() then
		 local childNode = node:addChild(object:getName())
		 addChildrenToTree(object, childNode)
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

function quitCallback()
  NeoLua.engine:setActive(false)
end

function update(dt)

end

Editor.loadTranslationList()
Translator.swapTranslation("english.csv")

Editor.loadUI()
NeoLua.system:setWindowTitle("Neo Scene Editor")
infoLog("Loaded editor!")
