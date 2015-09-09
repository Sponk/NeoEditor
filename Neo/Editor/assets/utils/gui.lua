--- gui.lua - Contains primitives for loading GUI descriptions from Lua tables
-- Data structure example:
-- /code
--test = {
--  [1] = {
--     name = "win1",
--     type = "Window",
--     x = 40,
--     y = 60,
--     w = 500,
--     h = 200,
--     label = "THIS IS A WINDOW!!!!!",
--    
--     content = {
--      [1] = {name = "btn1", type = "Button", x = 10, y = 10, w = 100, h = 50, label = "Button", callback = "someCallback"},
--      [2] = {name = "input1", type = "InputField", x = 10, y = 80, w = 100, h = 50, label = "InputField"}
--     }    
--  },
--  
--  [2] = {
--    name = "menubar1",
--    type = "MenuBar",
--    height = 30,
--    content = {
--      [1] = {label = "File", 
--             content = {
--              [2] = {type = "Item", label = "Quit", callback = "quitCallback"},
--              [1] = {type = "Item", label = "Open", callback = "openCallback"}
--             }
--          },
--          
--      [2] = {label = "Edit", 
--             content = {
--              [1] = {type = "Item", label = "Copy", callback = "quitCallback"},
--              [2] = {type = "Item", label = "Paste", callback = "openCallback"}
--             }
--          },
--          
--      [3] = {label = "Help", 
--             content = {
--              [1] = {type = "Item", label = "Help", callback = "quitCallback"},
--              [2] = {type = "Item", label = "About", callback = "aboutCallback"}
--             }
--          }
--    }
--  }
-- }
-- Gui.loadFromTable(test)
-- /endcode

Gui = {}

local neo2d = NeoLua.Neo2DEngine.getInstance()
local canvas = neo2d:getCanvas(0)

local messageBoxTable = {
   [1] = {
	  name = "msgBox",
	  type = "Window",
	  x = 100,
	  y = 100,
	  w = 300,
	  h = 100,
	  label = "Message",

	  content = {
		 [1] = {name = "ok", type = "Button", x = 150-25, y = 70, w = 50, h = 20, label = tr("OK"), callback = "messageBoxOkCallback"},

		 [2] = {name = "label", type = "Label", x = 10, y = 10, w = 0, h = 0, label = ""}
	  }
   },
}

local msgBoxData = {}

function Gui.messageBox(title, text)

   if msgBoxData.msgBox["msgBox"].window:isVisible() then
	  infoLog("Can not create message box because one is already visible!")
	  return false
   end

   msgBoxData.msgBox["msgBox"].window:setLabel(tostring(title))
   msgBoxData.msgBox["msgBox"]["label"]:setLabel(tostring(text))
   msgBoxData.msgBox["msgBox"].window:setVisible(true)

   Gui.wm:selectWindow(msgBoxData.msgBox["msgBox"].window)

   return true
end

function messageBoxOkCallback()
   msgBoxData.msgBox["msgBox"].window:setVisible(false)
end

function Gui.setupWM()
	Gui.wm = NeoLua.WindowManager(0,0,0,0,"")
	canvas:addWidget(neo2d:addWidget(Gui.wm))

	msgBoxData.msgBox = Gui.loadFromTable(messageBoxTable)
	msgBoxData.msgBox["msgBox"].window:setVisible(false)
end

function Gui.loadWidget(v, p)
   if v.type == "Button" then
	  local btn = NeoLua.ThemedButton(v.x,v.y,v.w,v.h,v.label or "")
	  
	  btn:setScriptCallback(v.callback or "EmptyCallback")
	  btn:setUserData(v.userData or 0)
	  btn:setFontSize(v.fontSize or 12)
	  p:addWidget(btn)
	  
	  return btn
   elseif v.type == "CheckButton" then
       local btn = NeoLua.CheckButton(v.x,v.y,v.size,v.label or "")

       btn:setScriptCallback(v.callback or "EmptyCallback")
       btn:setFontSize(v.fontSize or 12)
       p:addWidget(btn)

       return btn

   elseif v.type == "InputField" then
	  local input = NeoLua.ThemedInputField(v.x, v.y, v.w, v.h, v.label or "")
	  p:addWidget(input)
	  
	  input:setFontSize(v.fontSize or 12)
	  input:setType(v.inputType or 0)
	  input:setMultiline(v.isMultiline or false)
	  input:setScriptCallback(v.callback or "EmptyCallback")
	  
	  return input
   elseif v.type == "Label" then
	  local input = NeoLua.Label(v.x, v.y, v.w, v.h, v.label or "")
	  p:addWidget(input)
	  input:setFontSize(v.fontSize or 12)
	  
	  return input

   elseif v.type == "Sprite" then
          local sprite = NeoLua.Sprite(v.x, v.y, v.w, v.h, v.file, v.label or "")
          p:addWidget(sprite)

          return input
	  
   elseif v.type == "List" then
	  local list = NeoLua.List(v.x, v.y, v.w, v.h, v.label or "")
	  p:addWidget(list)                  
	  
	  for k,l in ipairs(v.content) do
		 list:addEntry(l)
	  end

	  return list
   elseif v.type == "Tree" then
	  local tree = NeoLua.TreeView(v.x, v.y, v.w, v.h, v.label or "")
	  tree:setScriptCallback(v.callback or "EmptyCallback")
	  p:addWidget(tree)
	  return tree
	  
   elseif v.type == "Slider" then
	  local slider = NeoLua.Slider(v.x, v.y, v.w, v.h, v.range[1] or 0, v.range[2] or 100, v.direction or 0)
	  slider:setScriptCallback(v.callback or "EmptyCallback")
	  p:addWidget(slider)                  
	  return slider
	  
   elseif v.type == "ScrollPane" then
	  local scrollPane = NeoLua.ScrollPane(v.x, v.y, v.w, v.h)
	  
	  p:addWidget(scrollPane)                  
	  local retval = {type = "ScrollPane", widget = scrollPane}
	  
	  for k,l in ipairs(v.content) do
		 retval[l.name] = Gui.loadWidget(l, scrollPane)
	  end

	  return retval
   elseif v.type == "ScaleLayout" then
	  local layout = NeoLua.ScaleLayout(v.x, v.y, v.w, v.h)
	  
	  p:addWidget(layout)                  
	  local retval = {type = "ScaleLayout", widget = layout}
	  
	  for k,l in ipairs(v.content) do
		 retval[l.name] = Gui.loadWidget(l, layout)
	  end

	  return retval
   end
end

function Gui.loadFromTable(t)
  neo2d:setDefaultFontSize(12)
  
  local topLevel = {}
  
  for i,v in ipairs(t) do  
    if v.type == "Window" then
    
      local win = NeoLua.Window(v.x, v.y, v.w, v.h, v.label)
      local content = {window = win}
      
      Gui.wm:addWindow(win)
      topLevel[v.name] = content

	  for j,k in ipairs(v.content) do
		 content[k.name] = Gui.loadWidget(k, win)
	  end
	  
      -- Add all contained widgets
    --[[  for i,v in ipairs(v.content) do
        if v.type == "Button" then
          local btn = NeoLua.ThemedButton(v.x,v.y,v.w,v.h,v.label or "")
          
          btn:setScriptCallback(v.callback or "")
          btn:setUserData(v.userData or 0)
          btn:setFontSize(v.fontSize or 12)
          win:addWidget(btn)
          
          content[v.name] = btn
          
        elseif v.type == "InputField" then
          local input = NeoLua.ThemedInputField(v.x, v.y, v.w, v.h, v.label or "")
          win:addWidget(input)
          input:setFontSize(v.fontSize or 12)
                    
          content[v.name] = input
        elseif v.type == "Label" then
          local input = NeoLua.Label(v.x, v.y, v.w, v.h, v.label or "")
          win:addWidget(input)
          input:setFontSize(v.fontSize or 12)
                    
          content[v.name] = input
          
        elseif v.type == "List" then
          local list = NeoLua.List(v.x, v.y, v.w, v.h, v.label or "")
          win:addWidget(list)                  
          content[v.name] = list
          
          for k,l in ipairs(v.content) do
            list:addEntry(l)
          end
        elseif v.type == "Tree" then
          local tree = NeoLua.TreeView(v.x, v.y, v.w, v.h, v.label or "")
          win:addWidget(tree)                  
          content[v.name] = tree             
        elseif v.type == "Slider" then
          local slider = NeoLua.Slider(v.x, v.y, v.w, v.h, v.range[1] or 0, v.range[2] or 100, v.direction or 0)
          slider:setScriptCallback(v.callback or "")
          win:addWidget(slider)                  
          content[v.name] = slider
        elseif v.type == "ScrollPane" then
		   local scrollPane = NeoLua.ScrollPane(v.x, v.y, v.w, v.h)
		   
		   win:addWidget(scrollPane)                  
		   content[v.name] = scrollPane
		end				
	   end]]
    
    elseif v.type == "MenuBar" then
    
      local menubar = NeoLua.MenuBar(v.height)
      
      canvas:addWidget(neo2d:addWidget(menubar))
      topLevel[v.name] = menubar
           
      for i,k in ipairs(v.content) do
      
        local menu = NeoLua.Menu(0,0)
        menu:setLabel(k.label)
        
        for i,j in ipairs(k.content) do
          if j.type == "Item" then
            local btn = NeoLua.Button(0,0,0,0,j.label)
            btn:setScriptCallback(j.callback or "EmptyCallback")
            btn:setFontSize(j.fontSize or 12)
            
            menu:addEntry(btn)
          end
        end
        
        menubar:addEntry(menu)
      end    
    end
  end 
  return topLevel
end

function EmptyCallback()

end

Gui.setupWM()
