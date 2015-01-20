--- The Object3d class
-- An 'Object3d' instance is an object in a scene which can be
-- manipulated via its position, rotation or scale. The 'Object3d' class
-- is mostly used as the base class for other permutations like
-- 'OEntity' or 'OLight' which extend the usability for their specific use-case.

dofile("class.lua")

Object3d = class(
    function(object, nativeObject)

    	if nativeObject == nil then
    	    print("Error: Native object is nil! Can not construct Object3d object!")
    	    return nil
    	end

    	object.type = getObjectType(nativeObject)
    	object.nativeObject = nativeObject
    	object.position = getPosition(nativeObject)
    	object.rotation = getRotation(nativeObject)
    	object.scale = getScale(nativeObject)
    end
)

--- Finds the object with the given name in the current scene
-- and constructs a new Object3d instance for it.
--
-- Example:
--[[
function onSceneUpdate()
  -- Get the object with name "Entity0"
  object = Object3d.getObject("Entity0")
end
]]
--
--
-- name: The object name.
--
-- return: A new Object3d instance
--
function Object3d.getObject(name)
    local object = Object3d(getObject(name))
    return object
end

--- Returns the position of the object.
--
-- returns: A vec3 containing the position.
function Object3d:getPosition()
    return self.position
end

--- Returns the rotation of the object.
--
-- returns: A vec3 containing the rotation.
function Object3d:getRotation()
    return self.rotation
end

--- Returns the scale of the object.
--
-- returns: A vec3 containing the scale.
function Object3d:getScale()
    return self.scale
end

--- Sets the position of the object
--
-- pos: The new position as a vec3
function Object3d:setPosition(pos)
    self.position = vec3(pos[1], pos[2], pos[3])
    setPosition(self.nativeObject, pos)
end

--- Sets the rotation of the object
--
-- pos: The new rotation as a vec3
function Object3d:setRotation(rot)
    self.rotation = vec3(rot[1], rot[2], rot[3])
    setRotation(self.nativeObject, rot)
end

--- Sets the scale of the object
--
-- pos: The new scale as a vec3
function Object3d:setScale(scale)
    self.scale = vec3(scale[1], scale[2], scale[3])
    setScale(self.nativeObject, scale)
end

--- Translates the object by the given vec3
--
-- The mode parameter is optional and should contain "local" when a local translation
-- should be executed. If this parameter is not given the translation will be done in
-- world space.
--
-- Example:
--[[
function onSceneUpdate()
  -- Translate 50 units towards the global Z-Axis
  object:translate({0,0,50})

  -- Translate 50 units towards the local Y-Axis
  object:translate({0,50,0}, "local")
end
]]
--
-- vec: A vec3 containing the direction to translate towards.
--
-- mode: The mode of the translation. Either nil or "local"
function Object3d:translate(vec, mode)
    translate(self.nativeObject, vec, mode)
    self.position = getPosition(self.nativeObject)
end

--- Rotates the object by the given amount around the given axis.
--
-- The mode parameter is optional and should contain "local" when a local rotation
-- should be executed. If this parameter is not given the rotation will be done in
-- world space.
--
-- Example:
--[[
function onSceneUpdate()
  -- Rotate 10° globally around the Z-Axis
  object:rotate({0,0,1}, 10)

  -- Rotate 10° locally around the X-Axis
  object:rotate({1,0,0}, 10, "local")
end
]]
--
-- axis: A vec3 containing the axis to rotate around
--
-- amount: The amount of how much to rotate
--
-- mode: The mode of the rotation. Either nil or "local"
function Object3d:rotate(axis, amount, mode)
    rotate(self.nativeObject, axis, amount, mode)
    self.rotation = getRotation(self.nativeObject)
    updateMatrix(self.nativeObject)
end

--- Returns the parent of the object or 'nil' if there isn't any.
--
-- return: The parent object as Object3d.
function Object3d:getParent()
    local nativeParent = getParent(self.nativeObject)

    if nativeParent == nil then
	   return nil
    end

    return Object3d(nativeParent)
end

--- Sets the parent of the object.
--
-- You can remove the parent by setting the parent to 'nil'
--
-- Example:
--[[
function onSceneUpdate()
  local newParent = Object3d.getObject("Parent")

  -- Set the new parent
  object:setParent(newParent)

  -- Remove parent
  object:setParent(nil)
end
]]
--
-- object: An Object3d instance that will be the new parent.
function Object3d:setParent(object)
    if object == nil then
    	setParent(self.nativeObject, nil)
    	return
    end

    setParent(self.nativeObject, object.nativeObject)
end

--- Returns the name of the object.
--
-- return: The name as a string
function Object3d:getName()
    return getName(self.nativeObject)
end

--- Activates an inactive Object3d
function Object3d:activate()
    activate(self.nativeObject)
end

--- Deactivates an inactive Object3d.
--
-- This function does _not_ delete the object from the scene!
-- The object will be invisible and physically inactive but 'not deleted'!
function Object3d:deactivate()
    deactivate(self.nativeObject)
end

--- Sets the activity status.
--
-- Calls either Object3d:activate or Object3d:deactivate to set the status.
--
-- Example:
--[[
function onSceneUpdate()
  -- Activate the object
  object:setActive(true)

  -- Deactivate the object
  object:setActive(false)
end
]]
-- status: A boolean representing the new status.
function Object3d:setActive(status)
    if status then
	self:activate()
    else
	self:deactivate()
    end
end

--- Returns the current activation status of the object.
--
-- return: A boolean.
function Object3d:isActive()
    return isActive(self.nativeObject)
end

--- Creates a clone of the object and returns it as an Object3d.
--
-- This will only clone the object and not the children.
-- Children need to be cloned manually!
--
-- return: The clone as Object3d
function Object3d:getClone()
    return Object3d(getClone(self.nativeObject))
end

--- Returns a table with all of the objects children.
--
-- The children are of the type Object3d.
--
-- return: A list of Object3d
function Object3d:getChildren()
    local nativeList = getChilds(self.nativeObject)
    local objects = {}

    for i = 1, #nativeList, 1 do
	     objects[i] = Object3d(nativeList[i])
    end

    return objects
end
