---The OLight class
--An 'OLight' instance is an light object in a scene which can be
--manipulated. The 'OLight' class is mostly used to manage lights in your
--3D scene.
--
--See also: <a href="Object3d.lua.html">Widget</a>

dofile("class.lua")
dofile("Object3d.lua")

---OLight(object3d)
--Create a new OLight object with the given Object3d
--as the base. Will return 'nil' if the given object is not of type 'Light'.
--
--If you give 'nil' as the base object it will create a new light in the current scene and
--return the new OLight object.
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
    	
    	object.intensity = getLightIntensity(object3d.nativeObject)
    	object.color = getLightColor(object3d.nativeObject)
    	object.radius = getLightRadius(object3d.nativeObject)
    	object.shadowsEnabled = false
    	object.shadowBlur = getLightShadowBlur(object3d.nativeObject)
    	object.shadowBias = getLightShadowBias(object3d.nativeObject)
    	object.spotAngle = getLightSpotAngle(object3d.nativeObject)
    end
)

---Returns the intensity of the light object.
--
--return: The intensity as a number.
function OLight:getIntensity()
    return self.intensity
end

---Sets the intensity of the light source
--
--You will be going for an intensity between 0.0 and 1.0 most of the time
--to prevent over saturation.
--
--intensity: The light intensity.
function OLight:setIntensity(intensity)
    setLightIntensity(self.nativeObject, intensity)
    self.intensity = intensity
end

---Sets the light color
--
--color: The new color as a vec3
function OLight:setColor(color)
    setLightColor(self.nativeObject, color)
    self.color = color
end

---Gets the light color
--
--return: The color as a vec3
function OLight:getColor()
    return self.color
end

---Sets the light radius in unit
--
--r: The new radius as a number.
function OLight:setRadius(r)
    setLightRadius(self.nativeObject, r)
    self.radius = r
end

---Gets the light radius
--
--return: The light radius as a number.
function OLight:getRadius()
    return self.radius
end

---Enables shadows
function OLight:enableShadows()
    self.shadowsEnabled = true
    enableShadow(self.nativeObject, 1)
end

---Disables shadows
function OLight:disableShadows()
    self.shadowsEnabled = false
    enableShadow(self.nativeObject, 0)
end

---Checks if the light has shadows enabled.
--
--return: A boolean value.
function OLight:hasShadows()
    return self.shadowsEnabled
end

---Sets the shadow blur.
--
--Keep in mind that higher values are slower to calculate
--since they cause more blur iterations in the GLSL shader!
--
--The value has to be be bigger than 0!
--
--b: The new blur value as an integer number > 0
function OLight:setShadowBlur(b)
    setLightShadowBlur(self.nativeObject, b)
    self.shadowBlur = b
end

---Returns the the shadow blur.
--
--return: The shadow blur as a number.
function OLight:getShadowBlur()
    return self.shadowBlur
end

---Sets the shadow bias
--
--b: The bias as a number.
function OLight:setShadowBias(b)
    setLightShadowBias(self.nativeObject, b)
    self.shadowBias = b
end

---Gets the shadow bias.
--
--return: The bias as a number.
function OLight:getShadowBias()
    return self.shadowBias
end

---Sets the spot angle.
--
--The angle needs to be given in degrees and 
--should be in the range 0 <= angle < 180.
--
--A value of 0° turns the light into a directional
--light source.
--
--A value of 180° turns the light into a point light.
--
--angle: The new angle as a number.
function OLight:setSpotAngle(angle)
    self.spotAngle = angle
    setLightSpotAngle(self.nativeObject, angle)
end

---Gets the spot angle
--
--return: The angle as a number in degrees.
function OLight:getSpotAngle()
    return self.spotAngle
end

---Turns the light into a directional light
function OLight:makeDirectional()
    self:setSpotAngle(0)
end

---Turns the light into a point light.
function OLight:makePointLight()
    self:setSpotAngle(180)
end
