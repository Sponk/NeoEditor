--- scene-tree-desc.lua - Contains the description for the scene tree dialog
local dlg = {
   [1] = {
	  name = "window",
	  type = "Window",
	  x = 10,
	  y = 70,
	  w = 300,
	  h = 650,
	  label = "Scene",
	  content = {
		 [1] = {
			name = "layout",
			type = "ScaleLayout",
			x = 0,
			y = 0,
			w = 0,
			h = 0,
			content = {
			   [1] = {
			   	  name = "scrollpanel", type = "ScrollPane", x = 0, y = 0,
			   	  w = 280, h = 210,
				  content = {			   		 
                                         [1] = {name = "tree", type = "Tree", x = 0, y = 0,
			   				w = 200, h = 0}
				  }			
			   }
			}
		 }
	  }
   }
}



return dlg
