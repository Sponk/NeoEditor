--- toolbar.lua - Contains the toolbar with the play/pause button and some other controls

local neo2d = NeoLua.Neo2DEngine.getInstance()
local data = {}
function data.create()
    local canvas = neo2d:getCanvas(0)
    
    local playbutton = NeoLua.Button(500, 500, 32, 32, "Play")
    playbutton:setScriptCallback("playGameCallback")
    canvas:addWidget(neo2d:addWidget(playbutton))
    
    data["playbutton"] = playbutton
    
    return data
end

function data.update()
	local res = NeoLua.system:getScreenSize()
	
	local size = data["playbutton"]:getSize()
	data["playbutton"]:setPosition(NeoLua.Vector2(res.x/2, res.y-size.y))
end

function playGameCallback()
	Editor.playGame("game.lua")
end

return data
