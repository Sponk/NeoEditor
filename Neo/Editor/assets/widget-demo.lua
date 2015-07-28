--- widget-demo.lua - A small demo window that contains all widgets that are implemented
return {
  [1] = {
    name = "demo-window",
    type = "Window",
    x = 10,
    y = 50,
    w = 750,
    h = 350,
    label = "Demo",
    content = {
      [1] = {name = "label", type = "Label", x = 10, y = 10, w = 0, h = 0, label = "Label"},
      [2] = {name = "button", type = "Button", x = 10, y = 300, w = 100, h = 40, label = "Button"},
      [3] = {name = "input", type = "InputField", x = 10, y = 50, w = 100, h = 20, label = "InputField"},
      [4] = {name = "list", type = "List", x = 10, y = 100, w = 200, h = 195,
        content = {"Entry1", "Entry2", "Entry3", "Entry4"}
      },
      
      [5] = {name = "tree", type = "Tree", x = 220, y = 100, w = 200, h = 195, label = ""},
      [6] = {name = "slider", type = "Slider", x = 220, y = 50, w = 200, h = 1, range = {0,100},
            callback = "sliderCallback"},
      [7] = {name = "slider2", type = "Slider", x = 430, y = 50, w = 1, h = 200, range = {0,100},
			 direction = NeoLua.SLIDER_VERTICAL},

	  [8] = {name = "scrollpanel", type = "ScrollPane", x = 450,
			 y = 50, w = 250, h = 150, content = {
				[1] = {
				   name = "label2", type = "Label",
				   x = 10, y = 10, w = 0, h = 0, label = "Label"
				},

				[2] = {name = "button2", type = "Button", x = 10, y = 40, w = 100, h = 40, label = "Button"},
				[3] = {name = "button3", type = "Button", x = 10, y = 150, w = 200, h = 40, label = "Es gibt jetzt UTF-8! ÄÖÜß!!!"}
			 }
			 
	  }
    }
  }
}
