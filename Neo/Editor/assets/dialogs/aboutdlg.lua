--- aboutdlg.lua - Contains the description for the about dialog

local dlg = {
  [1] = {
    name = "window",
    type = "Window",
    x = 10,
    y = 10,
    w = 300,
    h = 350,
    label = "About",
    content = {
      [1] = {name = "label", type = "Label", x = 10, y = 10, w = 0, h = 0, label = "About:"},
      [2] = {name = "button", type = "Button", x = 10, y = 300, w = 100, h = 40, label = "Close", callback = "closeAboutCallback"}
    }
  }
}

local aboutDlgTable = dofile("dialogs/aboutdlg.lua")
local aboutDlg = Gui.loadFromTable(aboutDlgTable)

aboutDlg["window"].window:setVisible(false)
function aboutCallback()

  local system = NeoLua.NeoEngine.getInstance():getSystemContext()
  local res = system:getScreenSize()
  local win = aboutDlg["window"].window
  local size = win:getSize()  
  
  win:setPosition(NeoLua.Vector2(res.x/2 - size.x/2, res.y/2 - size.y/2))
  win:setVisible(true)
end

function closeAboutCallback()
  aboutDlg["window"].window:setVisible(false)
end

return dlg