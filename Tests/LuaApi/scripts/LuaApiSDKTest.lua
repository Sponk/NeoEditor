dofile("SDK/Graphics2D.lua")
dofile("luaunit.lua")

loadCameraSkybox(getCurrentCamera(), "")

local sprite = Sprite(0,0,70,70,"maps/neo-icon.png")
mainCanvas:addWidget(sprite)

function onSceneUpdate()
    sprite:rotate(2)
end
