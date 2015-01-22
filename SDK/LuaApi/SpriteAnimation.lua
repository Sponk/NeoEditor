dofile("class.lua")

--- The SpriteAnimation class
-- Contains the primitives to play animations using a sprite sheet.
--
---- Class members
-- start: First frame of the animation
-- 
-- length: The length of the animation. The last frame shown is (start + length)
-- 
-- delay: The delay between frames in milliseconds
-- 
-- looping: A boolean which contains whether or not too loop the animation.
-- 
-- currentFrame: the current frame
-- 
-- lastTime: The last time the frame got switched. DO NOT MODIFY!


--- SpriteAnimation(start,length,delay,looping)
-- Create a SpriteAnimation object.
--
-- start: The first frame of the animation in the sprite sheet.
--
-- length: The length of the animation in frames.
--
-- delay: The delay between frames in milliseconds.
--
-- looping: A boolean specifying if the animation should loop after finishing.
SpriteAnimation = class(
	function(object, start, length, delay, looping)
	    object.start = start
	    object.length = length
	    object.delay = delay
	    object.looping = looping
	    object.currentFrame = 0

	    object.lastTime = getSystemTick()
	end)

--- Calculates the next frame in the animation
--
-- and returns the new frame number in the sprite sheet as a number.
function SpriteAnimation:nextFrame()

    -- Only update if the delay is over
    if getSystemTick() < self.lastTime + self.delay then
	     return self.start + self.currentFrame
    end

    self.lastTime = getSystemTick()

    -- Calculate the next frame
    local nframe = self.currentFrame + 1

    if nframe > self.length then
    	if self.looping then
    	    self.currentFrame = nframe % self.length    	    
    	end
    else
	     self.currentFrame = nframe
    end

    -- Add the start offset to the current frame
    return self.start + self.currentFrame
end
