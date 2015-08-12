--- widget-demo.lua - A small demo window that contains all widgets that are implemented

function testButtonCallback()
   Gui.messageBox("Message!", "Falsches Üben von Xylophonmusik \nquält jeden größeren Zwerg")
end


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
      [3] = {name = "input", type = "InputField", x = 10, y = 50, w = 150, h = 200, label = "InputField"},
      --[4] = {name = "list", type = "List", x = 10, y = 200, w = 140, h = 120,
      --  content = {"Entry1", "Entry2", "Entry3", "Entry4"}
      --},
      
      [4] = {name = "tree", type = "Tree", x = 220, y = 100, w = 200, h = 195, label = ""},
      [5] = {name = "slider", type = "Slider", x = 220, y = 50, w = 200, h = 1, range = {0,100},
            callback = "sliderCallback"},
      [6] = {name = "slider2", type = "Slider", x = 430, y = 50, w = 1, h = 200, range = {0,100},
			 direction = NeoLua.SLIDER_VERTICAL},

	    [7] = {name = "scrollpanel", type = "ScrollPane", x = 450,
			 y = 50, w = 280, h = 210, content = {
				[1] = {
				   name = "label2", type = "Label",
				   x = 10, y = 10, w = 0, h = 0, label = "Label"
				},

				[2] = {
				   name = "button", type = "Button",
				   x = 10, y = 230, w = 70, h = 30, label = "Message!",
				   callback = "testButtonCallback"
				},

				[3] = {name = "label", type = "Label", x = 10, y = 50, w = 0, h = 0, label = "„Pchnąć w tę łódź jeża \nlub ośm skrzyń fig. \n\nOh, welch Zynismus!“, \nquiekte Xavers jadegrüne Bratpfanne.\
\n\nFalsches Üben von Xylophonmusik \nquält jeden größeren Zwerg\
\n\nJanne var bördig nog att \nflö sig när Fredde skulle gå på tåget änna."}
			 }
			 
	  }
    }
  }
}

