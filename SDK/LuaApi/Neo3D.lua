--- The Neo3D Lua API
-- The Neo3D Lua interface delivers an easy to use and object oriented way of
-- accessing the script API.
--
-- Simply include this file ('SDK/Neo3D.lua') into your Lua script to use it.
--
-- This API is not installed by default in your project directory. To install it, use
-- the ProjectManager and click the option '"Copy Lua SDK"' after importing your project.
--
-- <h2>Usage</h2>
-- Usage of this API is fairly easy. Almost all functionality is about accessing and manipulating
-- Object3d's to create the in-game behavior you want.
--
-- <strong>Example:</strong>
-- /code
-- dofile("SDK/Neo3D.lua")
--
-- object = Object3d.getObject("Entity0")
--
-- function onSceneUpdate()
--     -- Rotate the object
--     object:rotate({0,1,0}, 2)
-- end
-- /endcode
--
-- The API also contains primitives for accessing the 2D functionality of Neo.
-- To use it simply include 'SDK/Graphics2D.lua' into your game script.
--
-- <strong>Example:</strong>
-- /code
-- dofile("SDK/Graphics2D.lua")
--
-- -- Load a sprite
-- sprite = Sprite(20,20,90,90,"maps/sprite.png", "")
--
-- -- Add sprite to the main canvas
-- mainCanvas:addWidget(sprite)
--
-- function onSceneUpdate()
--    -- Rotate sprite by 2 degrees every frame
--    sprite:rotate(2)
--  end
-- /endcode

dofile("class.lua")
dofile("Object3d.lua")
dofile("OLight.lua")
dofile("OEntity.lua")
dofile("OCamera.lua")
dofile("OSound.lua")

scriptDebug = false

if enableNeo3DTests then
    dofile("Graphics2D.lua")
    dofile("Testing/Tests.lua")
    dofile("Testing/LuaUnit.lua")

    scriptDebug = true

    Neo3D = {
		runTests = function()
		    local lu = LuaUnit.new()
		    lu:setOutputType("google")
		    lu:runSuite("Test3DSDK", "Test2DSDK", "TestModules")
		    --lu:runSuite("-o",  "junit", "-n", "test.xml")
		end
	}
end

