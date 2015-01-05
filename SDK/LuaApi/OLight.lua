--- The OLight class
-- An 'OLight' instance is an light object in a scene which can be
-- manipulated. The 'OLight' class is mostly used to manage lights in your
-- 3D scene.

dofile("class.lua")
dofile("Object3d.lua")

--- OLight(object3d)
-- Create a new OLight object with the given Object3d
-- as the base. Will return 'nil' if the given object is not of type 'Light'.
OLight = class(Object3d,
    function(object, object3d)
    
      if object3d == nil then
        print("Creating new light in the current scene.")
        
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

function OLight:getIntensity()
    local var = getLightIntensity(self.nativeObject)
    return var
end

function OLight:setIntensity(intensity)
    setLightIntensity(self.nativeObject, intensity)
end
