require("NeoGui")

local gui = NeoGui.GuiSystem.getInstance()
gui:setEnabled(true)

local button = NeoGui.ThemedButton(100, 100, 300, 30, "This is a button!")
local label = NeoGui.Label(100, 200, 0, 30, "This is a label!")

local labelhandle = gui:addWidget(label)
local buttonhandle = gui:addWidget(button)

local canvas = gui:getCanvas(0)
canvas:addWidget(buttonhandle)
canvas:addWidget(labelhandle)

function buttonCallback()
    print("Callback!")
end
button:setScriptCallback("buttonCallback")

local lastDelta = 0.0
function onSceneUpdate(delta)
    label:setLabel("Delta: " .. delta*1000 .. "\nDelta Skew: " .. 100*(1000*lastDelta / 1000*delta) .. "%")
    lastDelta = delta
end
