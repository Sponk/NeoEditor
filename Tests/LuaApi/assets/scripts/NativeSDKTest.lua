require("NeoLua")

local engine = NeoLua.Neo2DEngine.getInstance()
engine:setEnabled(true)

inputField = NeoLua.InputField(100, 300, 300, 30, "This is an text input!");
button = NeoLua.ThemedButton(100, 100, 300, 30, "This is a button!")
label = NeoLua.Label(100, 200, 0, 30, "This is a label!")

labelhandle = engine:addWidget(label)
buttonhandle = engine:addWidget(button)
inputhandle = engine:addWidget(inputField)

canvas = engine:getCanvas(0)
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
