local demo = dofile("widget-demo.lua")
local demo = Gui.loadFromTable(demo)

local tree = demo["demo-window"]["tree"]
local model = tree:getTreeModel()

local elem1 = model:addChild("Element1")
model:addChild("Element2")
local elem3 = model:addChild("Element3")

elem1:addChild("Child1")
elem1:addChild("Child2")
elem1:addChild("Child3")

local child6 = elem3:addChild("Child6")
elem3:addChild("Child7")

child6:addChild("SubChild1")
child6:addChild("SubChild2")
child6:addChild("SubChild3")

tree:setScriptCallback("treeCallback")
function treeCallback()
  infoLog("Tree selection changed: " .. tree:getSelected())
end

function sliderCallback()
  demo["demo-window"]["label"]:setLabel("Slider: " .. demo["demo-window"]["slider"]:getValue())
end

tree:setAutoSize(false)