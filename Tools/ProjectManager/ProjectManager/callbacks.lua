--- callbacks.lua - Contains all business logic

local ffi = require("ffi")
ffi.cdef[[
int chmod(const char *pathname, unsigned int mode);
]]

--- Code for making a file executable on UNIX like systems
local S_IRWXU = 508

--- Internal data
local data = { list = 0, projects = {} }
local lcd = require("LuaCommonDlg")

--- Home directory path
local homedir = os.getenv("HOME") or os.getenv("HOMEPATH")
local operatingSystem = "UNIX"
local seperator = "/"
local playerExec = "NeoPlayer"

--- Registry directory
local regdir = (os.getenv("HOME") or os.getenv("APPDATA")) .. seperator .. ".neo-projects"

-- WINDOWS!
if homedir:find("/") ~= 1 then
    operatingSystem = "WINDOWS"
    seperator = "\\"
    playerExec = playerExec .. ".exe"
end

--- Appends the project file extension to the given file name if it is not there yet.
-- @param filename The file path to extend
-- @return The extended file name
local function appendProjectExtension(filename)
    if filename:find(".mproj") ~= filename:len() - string.len(".mproj") + 1 then
        return filename .. ".mproj"
    end

    return filename
end

--- Copies all files needed to create a proper project
-- to the given project path.
--
-- @param projectPath The project to copy all files to.
function copyFiles(projectPath)
    -- FIXME: Don't throw Linux and OS X together!
    if operatingSystem == "UNIX" then
        NeoLua.copyDirFiles(".", projectPath, ".so")
        NeoLua.copyDirFiles(".", projectPath, ".dylib")

        NeoLua.copyFile("NeoPlayer", projectPath .. seperator .. "NeoPlayer")
        ffi.C.chmod(projectPath .. seperator .. "NeoPlayer", S_IRWXU)
    else
        local winPath = projectPath:gsub("\\", "/")
        NeoLua.copyDirFiles(".", winPath, ".dll")
        NeoLua.copyFile("NeoPlayer.exe", winPath .. "/NeoPlayer.exe")
    end
end

--- Creates a new project and adds the project to the registry.
function createProjectCallback()
    local fname = lcd.getSaveFilename(homedir, "mproj")
    if fname == nil then return end

    fname = appendProjectExtension(fname)

    local projectPath, projectName, extension = string.match(fname, "(.-)([^\\/]-%.?([^%.\\/]*))$")
    data.projects[projectName] = fname

    NeoLua.copyDirectory("./assets", projectPath:gsub("\\", "/") .. "/assets")
    NeoLua.copyDirectory("./translations", projectPath:gsub("\\", "/") .. "/translations")

    copyFiles(projectPath)

    local f = assert(io.open(fname, "w"))

    f:write([[
        <Neo version="0.5">
        <Project>
            <renderer name="StandardRenderer"/>
            <start file=""/>
        </Project>
        </Neo>
    ]])

    data.updateList()
    data.saveRegistry()
end

function updateEngineScriptsCallback()
    local fname = data.list:getSelected()
    if fname == nil then infoLog("Won't update engine scripts!") return end

    local projectPath, projectName, extension = string.match(data.projects[fname], "(.-)([^\\/]-%.?([^%.\\/]*))$")

    NeoLua.copyDirectory("./assets", projectPath:gsub("\\", "/") .. "/assets")
    NeoLua.copyDirectory("./translations", projectPath:gsub("\\", "/") .. "/translations")
end

--- Removes a project from the registry.
function removeProjectCallback()
    local fname = data.list:getSelected()
    if fname == nil then return end

    data.projects[fname] = nil
    data.updateList()
    data.saveRegistry()
end

--- Opens a project in the editor installed in the project directory.
function openProjectCallback()
    local fname = data.list:getSelected()

    if fname == nil or data.projects[fname] == nil then
        fname = lcd.getOpenFilename(homedir, "mproj")

        if fname == nil then
            return
        end

        local projectPath, projectName, extension = string.match(fname, "(.-)([^\\/]-%.?([^%.\\/]*))$")
        data.list:getTreeModel():addChild(projectName)
        data.projects[projectName] = fname

        data.saveRegistry()

        -- UUUUUGLY!
        os.execute("cd " .. projectPath .. " && " .. projectPath .. seperator .. playerExec)
        return
    end

    local projectPath, projectName, extension = string.match(data.projects[fname], "(.-)([^\\/]-%.?([^%.\\/]*))$")

    -- UUUUUGLY!
    os.execute("cd " .. projectPath .. " && " .. projectPath .. seperator .. playerExec)
end

--- Updates the engine used by the project.
function updateProjectCallback()

    local fname = data.list:getSelected()
    if fname == nil then return end

    local projectPath, projectName, extension = string.match(data.projects[fname], "(.-)([^\\/]-%.?([^%.\\/]*))$")
    copyFiles(projectPath)
end

--- Imports an existing project into the project registry.
function importProjectCallback()
    fname = lcd.getOpenFilename(homedir, "mproj")

    if fname == nil then
        return
    end

    local projectPath, projectName, extension = string.match(fname, "(.-)([^\\/]-%.?([^%.\\/]*))$")
    data.list:getTreeModel():addChild(projectName)
    data.projects[projectName] = fname

    data.saveRegistry()
    return
end

--- Saves the registry to the regdir.
function data.saveRegistry()
    local confdir = (os.getenv("HOME") or os.getenv("APPDATA")) .. seperator .. ".neo-projects"

    if not NeoLua.isFileExist(confdir) then
        NeoLua.createDirectory(confdir)
    end

    local f = assert(io.open(confdir .. seperator .. "registry.lua", "w"))

    f:write("return {")
    for k,v in pairs(data.projects) do
        f:write("[\"" .. k .. "\"] = \"" .. v:gsub("\\", "\\\\") .. "\",\n")
    end
    -- We need to ignore the last comma
    f:write("[0] = nil}")
end

--- Loads the registry.
function data.loadRegistry()
    local confdir = regdir .. seperator .. "registry.lua"

    if not NeoLua.isFileExist(confdir) then
        return
    end

    local reg = dofile(confdir)
    if reg ~= nil then
        data.projects = reg
        data.updateList()
    end
end

--- Updates the list model
-- ATTENTION: Needs data.list to be set first!
function data.updateList()
    -- Update list view
    local root = data.list:getTreeModel()
    root:clearChildren()

    for k,v in pairs(data.projects) do
        root:addChild(k)
    end
    data.list:selectEntry(root:getChild(0))
end

return data