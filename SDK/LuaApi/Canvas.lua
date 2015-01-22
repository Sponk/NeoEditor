dofile("class.lua")

--- The Canvas class
-- This class represents a Neo canvas that you can draw to.
-- A canvas holds all widgets and sprites and renders them every frame.

--- Canvas()
-- Creates a new Canvas object.
Canvas = class(
	function(object, canvas)
		if canvas ~= nil then
		    object.canvas = canvas
		else
		    object.canvas = createCanvas()
		end
	end
)

mainCanvas = Canvas(getMainCanvas())

--- Adds a Widget to the Canvas.
--
-- widget: The Widget object. See also Widget.
function Canvas:addWidget(widget)
    addWidgetToCanvas(self.canvas, widget.widget)
end
