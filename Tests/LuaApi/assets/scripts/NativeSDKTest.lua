require("Neo")

local gui = Neo.GuiSystem.getInstance()
gui:setEnabled(true)

inputField = Neo.InputField(100, 300, 300, 30, "This is an text input!");
button = Neo.ThemedButton(100, 100, 300, 30, "This is a button!")
label = Neo.Label(100, 200, 0, 30, "This is a label!")

labelhandle = gui:addWidget(label)
buttonhandle = gui:addWidget(button)
inputhandle = gui:addWidget(inputField)

canvas = gui:getCanvas(0)
canvas:addWidget(buttonhandle)
canvas:addWidget(labelhandle)
canvas:addWidget(inputhandle)

function buttonCallback()
    print("Callback!")
end
button:setScriptCallback("buttonCallback")

local lastDelta = 0.0
function onSceneUpdate(delta)
    label:setLabel("Delta: " .. delta*1000 .. "ms\nDelta Skew: " .. (1000*lastDelta - 1000*delta) .. "ms")
    lastDelta = delta
end
