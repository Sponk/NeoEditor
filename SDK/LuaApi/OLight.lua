--- The OLight class
-- An 'OLight' instance is an light object in a scene which can be
-- manipulated. The 'OLight' class is mostly used to manage lights in your
-- 3D scene.

dofile("class.lua")
dofile("Object3d.lua")

--- OLight(object3d)
-- Create a new OLight object with the given Object3d
-- as the base. Will return 'nil' if the given object is not of type 'Light'.
-- 
-- If you give 'nil' as the base object it will create a new light in the current scene and
-- return the new OLight object.
OLight = class(Object3d,
    function(object, object3d)
    
      if object3d == nil then        
        object3d = {}
        object3d.nativeObject = createLight()    
        object3d.type = "Light"
      end

	    if object3d.type ~= "Light" then
	     object = nil
	     print("Error: Base object is not of type 'Light'!")
	     return
    	end
    
    	object.type = "Light"
    	object.nativeObject = object3d.nativeObject
    	object.position = getPosition(object.nativeObject)
    	object.rotation = getRotation(object.nativeObject)
    	object.scale = getScale(object.nativeObject)
    end
)

--- Returns the intensity of the light object.
-- 
-- return: The intensity as a number.
function OLight:getIntensity()
    local var = getLightIntensity(self.nativeObject)
    return var
end

--- Sets the intensity of the light source
-- 
-- You will be going for an intensity between 0.0 and 1.0 most of the time
-- to prevent over saturation.
-- 
-- intensity: The light intensity.
function OLight:setIntensity(intensity)
    setLightIntensity(self.nativeObject, intensity)
end
