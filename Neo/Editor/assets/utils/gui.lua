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

Gui.wm = NeoLua.WindowManager(0,0,0,0,"")

local neo2d = NeoLua.Neo2DEngine.getInstance()
local canvas = neo2d:getCanvas(0)
canvas:addWidget(neo2d:addWidget(Gui.wm))

function Gui.loadFromTable(t)
  neo2d:setDefaultFontSize(12)
  
  local topLevel = {}
  
  for i,v in ipairs(t) do  
    if v.type == "Window" then
    
      local win = NeoLua.Window(v.x, v.y, v.w, v.h, v.label)
      local content = {window = win}
      
      Gui.wm:addWindow(win)
      topLevel[v.name] = content
    
      -- Add all contained widgets
      for i,v in ipairs(v.content) do
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
        end
      end
    
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
            btn:setScriptCallback(j.callback or "")
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