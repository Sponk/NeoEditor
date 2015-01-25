--- The OCamera class
-- An 'OCamera' instance is an camera object in a scene which can be
-- manipulated.
-- 
-- See also: <a href="Object3d.lua.html">Widget</a>

dofile("class.lua")

--- OCamera(object3d)
-- Create a new OCamera object with the given Object3d
-- as the base. Will return 'nil' if the given object is not of type 'Camera'.
OCamera = class(
    function(object, object3d)

      if object3d ~= nil and objec3d.nativeObject ~= nil then
        object.nativeObject = object3d.nativeObject
      else
        object.nativeObject = createCamera()
      end

    	object.type = getObjectType(object.nativeObject)
    	object.nativeObject = object.nativeObject
    	object.position = getPosition(object.nativeObject)
    	object.rotation = getRotation(object.nativeObject)
    	object.scale = getScale(object.nativeObject)
    	
    	object.color = getCameraClearColor(object.nativeObject)
    	object.fov = getCameraFov(object.nativeObject)
    	object.nearPlane = getCameraNear(object.nativeObject)
    	object.farPlane = getCameraFar(object.nativeObject)
    	object.fogDistance = getCameraFogDistance(object.nativeObject)
    	object.ortho = false
    	object.fog = false
    end
)

--- Sets the clear color
-- 
-- color: A vec3
function OCamera:setClearColor(color)
  self.color = color  
  setCameraClearColor(self.nativeObject, color)
end

--- Returns the clear color
-- 
-- return: A vec3
function OCamera:getClearColor()
  return self.color 
end

--- Changes the FoV
-- 
-- fov: A number
function OCamera:setFov(fov)
  self.fov = fov
  setCameraFov(self.nativeObject, fov)
end

--- Returns the FoV
-- 
-- return: A number
function OCamera:getFov()
  return self.fov
end

--- Changes the distance to the near plane
-- 
-- plane: A number
function OCamera:setNearPlane(plane)
  self.nearPlane = plane
  setCameraNear(self.nativeObject, plane)
end

--- Returns the distance to the near plane
-- 
-- return: A number
function OCamera:getNearPlane()
  return self.nearPlane
end

--- Changes the distance to the far plane
-- 
-- plane: A number
function OCamera:setFarPlane(plane)
  self.farPlane = plane
  setCameraFar(self.nativeObject, plane)
end

--- Returns the distance to the far plane
-- 
-- return: A number
function OCamera:getFarPlane()
  return self.farPlane
end

--- Changes the distance to the fog
-- 
-- distance: A number
function OCamera:setFogDistance(distance)
  self.fogDistance = distance
  setCameraFogDistance(self.nativeObject, distance)
end

--- Returns the distance to the fog
-- 
-- return: A number
function OCamera:getFogDistance()
  return self.fogDistance
end

--- Makes the camera ortho according to the value
-- 
-- value: A boolean value
function OCamera:enableOrtho(value)
  self.ortho = value
  enableCameraOrtho(self.nativeObject, value)
end

--- Returns if the camera is ortho
--
-- return: A boolean
function OCamera:isOrtho()
  return self.ortho
end

--- Turns on the camera fog according to the value
-- 
-- value: A boolean value
function OCamera:enableFog(value)
  self.fog = value
  enableCameraFog(self.nativeObject, value)
end

--- Returns if the camera has fog
--
-- return: A boolean
function OCamera:hasFog()
  return self.fog
end
