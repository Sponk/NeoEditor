--- menubar.lua - Contains the menubar description
local menubar = {
   [1] = {
	  name = "menubar1",
	  type = "MenuBar",
	  height = 30,
	  content = {
		 [1] = {label = "File", 
				content = {
				   [3] = {type = "Item", label = "Quit", callback = "quitCallback"},
				   [2] = {type = "Item", label = "Save", callback = "saveCallback"},
				   [1] = {type = "Item", label = "Open", callback = "openCallback"}
				}
		 },
		 
		 [2] = {label = "Edit", 
				content = {
				   [1] = {type = "Item", label = "Copy", callback = "quitCallback"},
				   [2] = {type = "Item", label = "Paste", callback = "openCallback"}
				}
		 },
		 
		 [3] = {label = "Help", 
				content = {
				   [1] = {type = "Item", label = "Help", callback = "quitCallback"},
				   [2] = {type = "Item", label = "About", callback = "aboutCallback"}
				}
		 }
	  }
   }
}

return menubar
