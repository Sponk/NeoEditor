--- callbacks.lua - Contains all business logic

local ffi = require("ffi")
ffi.cdef[[
int chmod(const char *pathname, unsigned int mode);
]]

local S_IRWXU = 508

local data = { list = 0, projects = {} }
local lcd = require("LuaCommonDlg")

local homedir = os.getenv("HOME") or os.getenv("HOMEPATH")
local operatingSystem = "UNIX"
local seperator = "/"
local playerExec = "NeoPlayer"

-- WINDOWS!
if homedir:find("/") ~= 1 then
    operatingSystem = "WINDOWS"
    seperator = "\\"
    playerExec = playerExec .. ".exe"
end

local function appendProjectExtension(filename)
    if filename:sub(1, -string.len(".mproj")) ~= ".mproj" then
        return filename .. ".mproj"
    end

    return filename
end

function createProjectCallback()
    local fname = lcd.getSaveFilename(homedir, "mproj")
    if fname == nil then return end

    fname = appendProjectExtension(fname)

    local projectPath, projectName, extension = string.match(fname, "(.-)([^\\/]-%.?([^%.\\/]*))$")
    data.projects[projectName] = fname

    NeoLua.copyDirectory("./assets", projectPath .. "/assets")

    -- FIXME: Don't throw Linux and OS X together!
    if operatingSystem == "UNIX" then
        NeoLua.copyDirFiles(".", projectPath, ".so")
        NeoLua.copyDirFiles(".", projectPath, ".dylib")

        NeoLua.copyFile("NeoPlayer", projectPath .. seperator .. "NeoPlayer")
        ffi.C.chmod(projectPath .. seperator .. "NeoPlayer", S_IRWXU)
    else
        NeoLua.copyDirFiles(".", projectPath, ".dll")
        NeoLua.copyFile("NeoPlayer.exe", projectPath .. seperator .. "NeoPlayer.exe")
    end

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

function removeProjectCallback()
    local fname = data.list:getSelected()
    if fname == nil then return end

    data.projects[fname] = nil
    data.updateList()

    data.saveRegistry()
end

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

function data.saveRegistry()
    local confdir = (os.getenv("HOME") or os.getenv("APPDATA")) .. seperator .. ".neo-projects"

    if not NeoLua.isFileExist(confdir) then
        NeoLua.createDirectory(confdir)
    end

    local f = assert(io.open(confdir .. seperator .. "registry.lua", "w"))

    f:write("return {")
    for k,v in pairs(data.projects) do
        f:write("[\"" .. k .. "\"] = \"" .. v .. "\",\n")
    end
    -- We need to ignore the last comma
    f:write("[0] = nil}")
end

function data.loadRegistry()
    local confdir = (os.getenv("HOME") or os.getenv("APPDATA")) .. seperator .. ".neo-projects" .. seperator .. "registry.lua"

    if not NeoLua.isFileExist(confdir) then
        return
    end

    local reg = dofile(confdir)
    if reg ~= nil then
        data.projects = reg
        data.updateList()
    end
end

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