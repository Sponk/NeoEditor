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
-- @param widget The Widget object. See also Widget.
function Canvas:addWidget(widget)
    addWidgetToCanvas(self.canvas, widget.widget)
end

--- Adds a batch to the Canvas.
-- @param widget The batch object. See also SpriteBatch.
function Canvas:addBatch(widget)
    addSpriteBatchToCanvas(self.canvas, widget.batch)
end
