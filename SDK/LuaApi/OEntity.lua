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

function OEntity:getPath()
  return self.path
end