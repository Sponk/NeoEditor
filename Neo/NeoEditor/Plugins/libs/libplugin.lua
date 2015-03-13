--- This library contains primitives for easy access to the Neo3D
-- editor plugin architecture.

require("plugins.libs.class")

Widget = class(
   function(self) end
)

function Widget:add(w)
   Fl_Add(self.nativeWidget, w.nativeWidget)
end

function Widget:setCallback(cb)
   Fl_SetCallback(self.nativeWidget, cb)
end

function Widget:getLabel()
   return Fl_GetLabel(self.nativeWidget)
end

function Widget:show()
   Fl_Show(self.nativeWidget)
end

Window = class(Widget,
   function(self, x, y, w, h, title)
	  self.x = x
	  self.y = y
	  self.w = w
	  self.h = h
	  self.title = title
	  
	  self.nativeWidget = Fl_CreateWindow(x,y,w,h,title)
   end
)

Button = class(Widget,
   function(self, x, y, w, h, title)
	  self.x = x
	  self.y = y
	  self.w = w
	  self.h = h
	  self.title = title
	  
	  self.nativeWidget = Fl_CreateButton(x,y,w,h,title)
   end
)

Input = class(Widget,
   function(self, x, y, w, h, title)
	  self.x = x
	  self.y = y
	  self.w = w
	  self.h = h
	  self.title = title
	  
	  self.nativeWidget = Fl_CreateInput(x,y,w,h,title)
   end
)

function Input:getValue()
   local str =  Fl_InputValue(self.nativeWidget)
   return str
end
