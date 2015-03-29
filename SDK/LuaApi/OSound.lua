--- The OSound class
-- An 'OSound' instance is a sound object in a scene which can be
-- used to play 3D and 2D sounds and music during runtime.
-- 
-- See also: <a href="Object3d.lua.html">Widget</a>

dofile("class.lua")
dofile("Object3d.lua")

--- OSound(file)
-- Creates a new sound object and loads the specified file.
--
-- @param file A string containing the path to a sound file.
OSound = class(Object3d,
   function(obj, file)
	  if not isstring(file) then error("OSound requires a file path!") end
	  
	  obj.nativeObject = createSound(file)
	  obj.gain = getSoundGain(obj.nativeObject)
	  obj.looping = getSoundLooping(obj.nativeObject)
	  
	  obj.type = getObjectType(obj.nativeObject)
	  obj.nativeObject = obj.nativeObject
	  obj.position = getPosition(obj.nativeObject)
	  obj.rotation = getRotation(obj.nativeObject)
	  obj.scale = getScale(obj.nativeObject)
   end
)

--- Sets sound gain.
-- @param g The new sound gain as a number.
function OSound:setGain(g)
   self.gain = g
   setSoundGain(self.nativeObject, g)
end

--- Sets if the sound should repeat in a loop.
-- @param looping A boolean.
function OSound:setLooping(looping)
   self.looping = looping
   setSoundLooping(self.nativeObject, tonumber(looping))
end

--- Plays the sound one time or until stopped or paused
-- if the sound is configure to loop.
function OSound:play()
   playSound(self.nativeObject)
end

--- Pauses the sound so it can be resumed later at the same
-- point.
function OSound:pause()
   pauseSound(self.nativeObject)
end

--- Stops the sound.
function OSound:stop()
   stopSound(self.nativeObject)
end
