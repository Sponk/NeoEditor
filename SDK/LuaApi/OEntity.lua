--- The OEntity class
-- An 'OEntity' instance is an entity object in a scene which can be
-- manipulated. The 'OEntity' class is mostly used to manage meshes in your 3D scene.
-- 
-- See also: <a href="Object3d.lua.html">Widget</a>

dofile("class.lua")
dofile("Object3d.lua")

--- OEntity(object3d)
-- Create a new OEntity object with the given Object3d
-- as the base. Will return 'nil' if the given object is not of type 'Entity'.
-- 
-- If you give a path as the base object it will load the new mesh and add it to the current scene.
OEntity = class(Object3d,
    function(object, object3d)
    
      if type(object3d) == "string" then        
        local nativeObject = loadMesh(object3d)
        
        if nativeObject == nil then
          print("Error: Could not load '" .. object3d .. "'")
          object = nil
          return
        end
        
        object3d = OEntity(Object3d(nativeObject))      
      end

      if object3d.type ~= "Entity" then
       object = nil
       print("Error: Base object is not of type 'Entity'!")
       return
      end
    
      object.type = "Entity"
      object.nativeObject = object3d.nativeObject
      object.position = getPosition(object.nativeObject)
      object.rotation = getRotation(object.nativeObject)
      object.scale = getScale(object.nativeObject)
      object.path = getMeshFilename(object.nativeObject)
    end
)

--- Returns the path to the loaded mesh file.
-- @return The path as a string.
function OEntity:getPath()
  return self.path
end

--- Returns the ID of the currently running animation.
-- @return The ID as a number
function OEntity:getCurrentAnimation()
   return getCurrentAnimation(self.nativeObject)
end

--- Changes the current animation and starts the new one.
-- @param id The ID of the new animation.
function OEntity:changeAnimation(id)
   changeAnimation(self.nativeObject, id)
end

--- Checks if the currently running animation has finished.
-- @return A boolean
function OEntity:isAnimationOver()
   return isAnimationOver(self.nativeObject)
end

--- Returns the current animation speed.
-- @return The animation speed as a number.
function OEntity:getAnimationSpeed()
   return getAnimationSpeed(self.nativeObject)
end

--- Changes the animation speed.
-- @param speed The new animation speed.
function OEntity:setAnimationSpeed(speed)
   setAnimationSpeed(self.nativeObject, speed)
end

--- Returns the current frame of the animation that is being
-- displayed.
-- @return The current frame number
function OEntity:getCurrentFrame()
   return getCurrentFrame(self.nativeObject)
end

--- Changes the current frame that is being displayed.
-- @param frame The index of the new frame.
function OEntity:setCurrentFrame(frame)
   return setCurrentFrame(self.nativeObject, frame)
end

--- Enables/Disables the shadow for this object.
-- @param enabled A boolean value
function OEntity:enableShadow(enabled)
   enableShadow(self.nativeObject, enabled)
end

--- Checks if the entity has a shadow.
-- @return A boolean value
function OEntity:hasShadow()
   return isCastingShadow(self.nativeObject)
end

--- Returns the linear damping value
-- @return The linear damping as a number
function OEntity:getLinearDamping()
   return getLinearDamping(self.nativeObject)
end

--- Changes the linear damping value
-- @param damping The value as a number
function OEntity:setLinearDamping(damping)
   setLinearDamping(self.nativeObject, damping)
end

--- Returns the angular damping
-- @return The angular damping as a number.
function OEntity:getAngularDamping()
   return getAngularDamping(self.nativeObject)
end

--- Changes the angular damping
-- @param damping the value as a number
function OEntity:setAngularDamping(damping)
   setAngularDamping(self.nativeObject, damping)
end

--- Returns the angular factor
-- @return The angular factor as a number.
function OEntity:getAngularFactor()
   return getAngularFactor(self.nativeObject)
end

--- Changes the angular factor
-- @param factor The new factor as a number
function OEntity:setAngularFactor(factor)
   setAngularFactor(self.nativeObject, factor)
end

--- Returns the linear factor
-- @return The linear factor as a vec3
function OEntity:getLinearFactor()
   return getLinearFactor(self.nativeObject)
end

--- Changes the linear factor
-- @param factor The linear factor as a vec3
function OEntity:setLinearFactor(factor)
   setLinearFactor(self.nativeObject, factor)
end

--- Returns the mass of the entity
-- @return The mass as a number
function OEntity:getMass()
   return getMass(self.nativeObject)
end

--- Changes the mass of the entity
-- @param mass The new mass as a number
function OEntity:setMass(mass)
   setMass(self.nativeObject, mass)
end

--- Changes the friction of the entity
-- @param fric The friction as a number.
function OEntity:setFriction(fric)
   setFriction(self.nativeObject, fric)
end

--- Returns the friction value
-- @return The friction value as a number.
function OEntity:getFriction()
   return getFriction(self.nativeObject)
end

--- Changes the restitution value
-- @param rest The new restitution value as a number
function OEntity:setRestitution(rest)
   setRestitution(self.nativeObject, rest)
end

--- Returns the restitution value
-- @return The restitution value as a number.
function OEntity:getRestitution()
   return getRestitution(self.nativeObject)
end

--- Clears the central force
function OEntity:clearForces()
   clearForces(self.nativeObject)
end

--- Applies a new central force to the given object.
--
-- The new force will be added to the currently existing central force of the
-- object. The mode parameter is optional and allows you to apply forces relative to the objects
-- current rotation.
--
-- @param force A vec3 containing the force to apply.
-- @param mode A string containing "local". Can be left out.
function OEntity:addCentralForce(force, mode)
   addCentralForce(self.nativeObject, force, mode)
end

--- Applies a new torque to the given object.
--
-- The new torque will be added to the currently existing torque of the
-- object. The mode parameter is optional and allows you to apply a torque relative to the objects
-- current rotation.
--
-- @param torque A vec3 containing the torque to apply.
-- @param mode A string containing "local". Can be left out.
function OEntity:addTorque(torque, mode)
   addTorque(self.nativeObject, torque, mode)
end

--- Returns the central force
-- @return The central force as a vec3
function OEntity:getCentralForce()
   return getCentralForce(self.nativeObject)
end

--- Returns the current torque
-- @return The torque as a vec3
function OEntity:getTorque()
   return getTorque(self.nativeObject)
end

--- Checks if the object is colliding with anything.
-- @return A boolean value
function OEntity:isColliding()
   return isCollisionTest(self.nativeObject)
end

--- Checks if the object is colliding with the given object.
-- @param object An OEntity object
-- @return A boolean
function OEntity:isCollidingWith(object)
   return isCollisionBetween(self.nativeObject, object.nativeObject)
end

--- Returns the number of collisions the object currently has.
-- @return The number of collisions as a number.
function OEntity:getNumCollisions()
   return getNumCollisions(self.nativeObject)
end

--- Enables physics for the entity
-- @param value A boolean value
function OEntity:enablePhysics(value)
   enablePhysics(self.nativeObject, value)
end
