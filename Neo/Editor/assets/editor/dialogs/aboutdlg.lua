--- aboutdlg.lua - Contains the description for the about dialog

local dlg = {
  [1] = {
    name = "window",
    type = "Window",
    x = 10,
    y = 10,
    w = 300,
    h = 300,
    label = "About",
    content = {
      [1] = {name = "label", type = "Label", x = 0, y = 145, w = 300, h = 0, label = tr("ABOUT_TEXT")},
      [2] = {name = "button", type = "Button", x = 100, y = 250, w = 100, h = 20, label = tr("Close"), callback = "closeAboutCallback"},
      [3] = {name = "sprite", type = "Sprite", x = 150-(128/2), y = 10, w = 128, h = 128, file = "assets/editor/neo-icon.png"}
    }
  }
}

local aboutDlg = Gui.loadFromTable(dlg)
local win = aboutDlg["window"].window
win:setVisible(false)
aboutDlg["window"]["label"]:setAlignment(NeoLua.TEXT_ALIGN_CENTER)

function aboutCallback()

  local system = NeoLua.NeoEngine.getInstance():getSystemContext()
  local res = system:getScreenSize()
  local size = win:getSize()  
  
  win:setPosition(NeoLua.Vector2(res.x/2 - size.x/2, res.y/2 - size.y/2))
  win:setVisible(true)
end

function closeAboutCallback()
  win:setVisible(false)
end

return dlg
