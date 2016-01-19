local res = NeoLua.system:getScreenSize()
local entityEditor = {}

local dlg = {
  [1] = {
    name = "window",
    type = "Window",
    x = res.x - 310,
    y = 70,
    w = 300,
    h = 650,
    label = tr("Edit Entity"),
    content = {
      [1] = {
        name = "nameLabel",
        type = "Label",
        x = 10,
        y = 10,
        w = 0,
        h = 0,
        label = tr("Name:")
      },
      
      [2] = {
        name = "nameInput",
        type = "InputField",
        x = 10,
        y = 20,
        w = 280,
        h = 20,
        label = "",
        callback = "updateEntityConfig"
      },
      
      [3] = {
        name = "positionLabel",
        type = "Label",
        x = 10,
        y = 50,
        w = 0,
        h = 0,
        label = tr("Position:")
      },
      
      [4] = {
        name = "pxInput",
        type = "InputField",
        x = 10,
        y = 60,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [5] = {
        name = "pyInput",
        type = "InputField",
        x = 85,
        y = 60,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [6] = {
        name = "pzInput",
        type = "InputField",
        x = 160,
        y = 60,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      
      [7] = {
        name = "rotationLabel",
        type = "Label",
        x = 10,
        y = 90,
        w = 0,
        h = 0,
        label = tr("Rotation:")
      },
      
      [8] = {
        name = "rxInput",
        type = "InputField",
        x = 10,
        y = 100,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [9] = {
        name = "ryInput",
        type = "InputField",
        x = 85,
        y = 100,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [10] = {
        name = "rzInput",
        type = "InputField",
        x = 160,
        y = 100,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      
      [11] = {
        name = "scaleLabel",
        type = "Label",
        x = 10,
        y = 130,
        w = 0,
        h = 0,
        label = tr("Scale:")
      },
      
      [12] = {
        name = "sxInput",
        type = "InputField",
        x = 10,
        y = 140,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [13] = {
        name = "syInput",
        type = "InputField",
        x = 85,
        y = 140,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [14] = {
        name = "szInput",
        type = "InputField",
        x = 160,
        y = 140,
        w = 60,
        h = 20,
        label = "",
        inputType = NeoLua.DECIMAL_INPUT,
        callback = "updateEntityConfig"
      },
      
      [15] = {
        name = "physicsPropertiesButton",
        type = "Button",
        x = 10,
        y = 185,
        w = 280,
        h = 20,
        label = tr("Physics Properties")--,
        --callback = ""      
      }
    }
  }
}