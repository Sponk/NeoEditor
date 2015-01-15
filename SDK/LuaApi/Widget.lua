dofile("class.lua")

--- The widget class
-- The widget class represents the base class for every widget.
-- It contains the most basic functionality shared by every widget type like
-- translating the position or changing the rotation.
--
--
---- Class members
-- 
-- <b>Do not write to these directly! Use the suitable methods for that!</b>
-- 
-- widget: The native widget handle used by the C++ backend.
-- 
-- position.x: The X position of the widget
-- 
-- position.y : The Y position of the widget
-- 
-- height: The height of the widget
-- 
-- width: The width of the widget
-- 
-- rotation: The current rotation of the widget.

--- Widget(x,y,w,h,text)
-- Creates a new Widget object.<br><br>
--
-- x: The x position on the canvas
--
-- y: The y position on the canvas
--
-- w: The width of the widget
--
-- h: The height of the widget
--
-- text: (optional) The label of the widget.
Widget = class(
	function(object, x, y, w, h, text)
		if text == nil then text = "" end
		object.widget = createSprite(x,y,w,h,file,text)
		object.position = {x=x,y=y}
		object.width = w
		object.height = h
		object.rotation = 0
    end
)

function Widget:setPositionX(value)
    self.position[1] = value
end

function Widget:setPostionY(value)
    self.position[2] = value
end

--global table for all collision objects
--collidable_objects = {}

--- Sets the current position of the widget
--
-- x: The new X position
--
-- y: The new Y position
function Widget:setPosition(x,y)
    self.position[1] = x
    self.position[2] = y

    setWidgetPosition(self.widget, self.position)
end

--- Sets the current rotation of the widget
--
-- r: The new rotation
function Widget:setRotation(r)
    self.rotation = r
    setWidgetRotation(self.widget, self.rotation)
end

--- Rotates the widget by an angle.
--
-- angle: The angle to rotate by
function Widget:rotate(angle)
    self:setRotation(self.rotation + angle)
end

--- Translates the widget by the given coordinates relative to the current position.
--
-- x: The X offset
--
-- y: The Y offset
function Widget:translate(x,y)
    self:setPosition(self.position[1] + x, self.position[2] + y)
end


--- Gets the current position of the widget.
--
-- x: The X cordonat
--
-- y: The Y cordonat
function Widget:getPosition()
    local pos = {}
    pos.x = self.position[1]
    pos.y = self.position[2]
    return pos
end

--Whether or not this object can collide with anathors
--call this in a load function !
function Widget:setCollidable(value,tag)
    self.collidable = value
    self.tag = tag
    if value  then 
        table.insert(collidable_objects,self)
    elseif not value then
        table.remove(collidable_objects,self)
    end
end
--Returns the Width/Height of this object
function Widget:getSize()
    local size = {}
    size.width = self.width
    size.height = self.height
    return size
end

--- Retrieves the current label of the widget.
function Widget:getLabel()
    return getLabel(self.widget)
end

--- Changes the current label of the widget.
--
-- label: The new label.
function Widget:setLabel(label)
    setLabel(self.widget, label)
end
