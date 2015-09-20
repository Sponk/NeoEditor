--- menubar.lua - Contains the menubar description
local menubar = {
   [1] = {
	  name = "menubar1",
	  type = "MenuBar",
	  height = 30,
	  content = {
		 [1] = {label = tr("File"),
				content = {
				   [3] = {type = "Item", label = tr("Quit"), callback = "quitCallback"},
				   [2] = {type = "Item", label = tr("Save"), callback = "saveCallback"},
				   [1] = {type = "Item", label = tr("Open"), callback = "openCallback"}
				}
		 },
		 
		 [2] = {label = tr("Edit"),
				content = {
				   [1] = {type = "Item", label = tr("Copy"), callback = "quitCallback"},
				   [2] = {type = "Item", label = tr("Paste"), callback = "openCallback"}
				}
		 },
		 
		 [3] = {label = tr("Help"),
				content = {
				   [1] = {type = "Item", label = tr("Help"), callback = "quitCallback"},
				   [2] = {type = "Item", label = tr("About"), callback = "aboutCallback"}
				}
		 }
	  }
   }
}

return menubar
