--- The Project Manager
-- The project manager is a small utility application that simplifies project creation and management.
-- Its main features are delivering an easy to use list of all projects you created or imported and
-- upgrading the Neo engine used in those projects.
--
-- Launch using "./NeoPlayer -e ProjectManager/main.lua" inside the editor installation directory.

require("NeoLua")
local Translator = require("utils.translator")

require("utils.utils")
require("utils.gui")
local ProjectManager = require("ProjectManager.callbacks")

local engine = NeoLua.NeoEngine.getInstance()
local system = engine:getSystemContext()

local neo2d = NeoLua.Neo2DEngine.getInstance()
local canvas = neo2d:getCanvas(0)

canvas:setClearColor(NeoLua.Vector4(0.85,0.85,0.85,1))

-- local label = NeoLua.Label(100, 100, 0, 0, "THIS IS A LABEL!")
-- canvas:addWidget(neo2d:addWidget(label))

local createProjectButton = NeoLua.ThemedButton(10, 420, 130, 20, tr("Create Project"))
createProjectButton:setScriptCallback("createProjectCallback")
canvas:addWidget(neo2d:addWidget(createProjectButton))

local removeProjectButton = NeoLua.ThemedButton(150, 420, 130, 20, tr("Remove Project"))
removeProjectButton:setScriptCallback("removeProjectCallback")
canvas:addWidget(neo2d:addWidget(removeProjectButton))

local openProjectButton = NeoLua.ThemedButton(300, 10, 290, 20, tr("Open Project"))
openProjectButton:setScriptCallback("openProjectCallback")
canvas:addWidget(neo2d:addWidget(openProjectButton))

local updateProjectButton = NeoLua.ThemedButton(300, 35, 290, 20, tr("Update Engine"))
updateProjectButton:setScriptCallback("updateProjectCallback")
canvas:addWidget(neo2d:addWidget(updateProjectButton))

local updateEngineScriptsButton = NeoLua.ThemedButton(300, 60, 290, 20, tr("Update Engine Scripts"))
updateEngineScriptsButton:setScriptCallback("updateEngineScriptsCallback")
canvas:addWidget(neo2d:addWidget(updateEngineScriptsButton))

local importProjectButton = NeoLua.ThemedButton(300, 85, 290, 20, tr("Import Project"))
importProjectButton:setScriptCallback("importProjectCallback")
canvas:addWidget(neo2d:addWidget(importProjectButton))

-- canvas:addWidget(neo2d:addWidget(NeoLua.Label(300, 40, 0, 0, tr("Name:"))))
-- local projectNameEdit = NeoLua.ThemedInputField(300, 50, 290, 20, "")
-- canvas:addWidget(neo2d:addWidget(projectNameEdit))

local scrollPane = NeoLua.ScrollPane(10, 10, 275, 400)
canvas:addWidget(neo2d:addWidget(scrollPane))

local listView = NeoLua.TreeView(0,0,0,0, "")
scrollPane:addWidget(listView)
ProjectManager.list = listView

ProjectManager.loadRegistry()

local root = listView:getTreeModel()

--for i = 0, 100, 1 do
--root:addChild("HAHAHAH")
--end

system:setScreenSize(600, 450)
system:setWindowTitle(tr("Project Manager"))

function update() end
