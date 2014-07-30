setPluginName("Prefabs")
setPluginAuthor("Yannick Pflanzer")
setPluginDescription("A plugin that allow you to load and save your object selection as a prefab.")

addEditorMenu("Load Prefab", "load_callback")
addEditorMenu("Save selection as Prefab", "save_callback")

require("plugins.libs.libxml")
--dofile("plugins/libs/libxml.lua")
        
-- From penlib
function split(s,re,plain,n)
    local find,sub,append = string.find, string.sub, table.insert
    local i1,ls = 1,{}
    if not re then re = '%s+' end
    if re == '' then return {s} end
    while true do
        local i2,i3 = find(s,re,i1,plain)
        if not i2 then
            local last = sub(s,i1)
            if last ~= '' then append(ls,last) end
            if #ls == 1 and ls[1] == '' then
                return {}
            else
                return ls
            end
        end
        append(ls,sub(s,i1,i2-1))
        if n and #ls == n then
            ls[#ls] = sub(s,i1)
            return ls
        end
        i1 = i3+1
    end
end

function normcase(p)
    return p;
end

function getos()
    local f = io.popen("uname", 'r')
    
    if f == nil then
        return "Windows"
    end
    
    local s = f:read('*a')
    f:close()
    
    if string.find(s, "Windows") ~= nil then
        return "Windows"
    end
    
    return "UNIX"
end

function currentdir()
    local curos = getos()
    
    if curos == "UNIX" then
        return os.getenv("PWD")
    end
    
    if curos == "Windows" then
        return os.getenv("CWD")
    end
end

if getos() == "Windows" then
    sep = "\\"
else
    sep = "/"
end

-- From penlib
function relpath(P,start)
   
    local split,normcase,min,append = split, normcase, math.min, table.insert
    P = normcase(P)
    start = start or currentdir()
    start = normcase(start)
    
    local startl, Pl = split(start,sep), split(P,sep)
    local n = min(#startl,#Pl)
    local k = n+1 -- default value if this loop doesn't bail out!
    for i = 1,n do
        if startl[i] ~= Pl[i] then
            k = i
            break
        end
    end
    
    local rell = {}
    for i = 1, #startl-k+1 do rell[i] = '..' end
    if k <= #Pl then
        for i = k,#Pl do append(rell,Pl[i]) end
    end
    return table.concat(rell,sep)
end

function getPath(path)
    return path:match("(.*"..sep..")")
end

function vec2str(vec)
    local output = ""
    
    for i = 1, #vec, 1 do
        output = output .. vec[i] .. " "
    end
    
    return output
end

function bool2str(bool)
    if bool then return "true" else return "false" end
end

function objectToXml(object, path)

    local objectType = getObjectType(object)
    local output = "<" .. objectType .. " name=\"" .. getName(object) .. "\""
   
    if objectType == "Entity" then
        output = output .. " file=\"" .. relpath(getMeshFilename(object), path)
        output = output .. "\">\n"
    
        local min = getBoundingMin(object)
        local max = getBoundingMax(object)
    
        output = output .. "<BoundingBox min=\"" .. vec2str(min) .. "\" max=\"" .. vec2str(max) .. "\"/>\n"    
    
        output = output .. "<ObjectProperties invisible=\"" .. bool2str(not isVisible(object)) .. "\"/>\n"
    
        output = output .. "<physics\n"
        
        -- TODO: Get real shape!
        output = output .. "\tshape=\"Box\"\n"
    
        -- TODO: Is it really a ghost?
        output = output .. "\tghost=\"false\"\n"
    
        output = output .. "\tmass=\"" .. getMass(object) .. "\"\n"
        output = output .. "\tfriction=\"" .. getFriction(object) .. "\"\n"
        output = output .. "\trestitution=\"" .. getRestitution(object) .. "\"\n"
        output = output .. "\tlinearDamping=\"" .. getLinearDamping(object) .. "\"\n"
        output = output .. "\tangularDamping=\"" .. getAngularDamping(object) .. "\"\n"
        output = output .. "\tangularFactor=\"" .. getAngularFactor(object) .. "\"\n"
        output = output .. "\tlinearFactor=\"" .. vec2str(getLinearFactor(object)) .. "\"\n"
    
        output = output .. "/>\n"
    end
    
    -- Position is relative to the selection center
    local position = (getSelectionCenter() - getPosition(object))
    local rotation = getRotation(object)
    local scale = getScale(object)
    
    output = output .. "<transform\n"
    output = output .. "\tposition=\"" .. vec2str(position) .. "\"\n"
    output = output .. "\trotation=\"" .. vec2str(rotation) .. "\"\n"
    output = output .. "\tscale=\"" .. vec2str(scale) .. "\"\n"
    output = output .. "/>\n"

    output = output .. "<active value=\"" .. bool2str(isActive(object)) .. "\"/>\n"


    output = output .. "</" .. objectType .. ">"

    return output
end

function save_callback()

    local selection = getCurrentSelection();
    
    if selection == nil then
        print("Selection is NIL!")
        return
    end
    
    if #selection == 0 then
       messagebox("No objects selected! Can't create prefab!") 
       return
    end

    local filename = saveFileDlg("Choose a file", getProjectDir(), "*.mp");
    
    if filename == nil then return end
    
    local path = getPath(filename)
    local content = "<prefab>"
       
    for i = 1, #selection, 1 do
       content = content .. objectToXml(selection[i], path)
    end
    
    content = content .. "</prefab>"
    
    -- print(content)
    
    local output = io.open(filename, "w")
    output:write(content)
    output:close()
end

function addEntity(entity)
    print("Got Entity: " .. entity["@name"])
end

function load_callback()

    local filename = openFileDlg("Choose a file", getProjectDir(), "*.mp")
    
    if filename == nil then return end
        
    local xml = newParser()
    local f = io.open(filename, "r")
    local content = f:read("*all")
    f:close()

    local objects = xml:ParseXmlText(content)
    local entities = {}
    
    if objects.prefab.Entity ~= nil then
        if #objects.prefab.Entity > 0 then  
            for i = 1, #entities, 1 do
                addEntity(entities[i])
            end
        else
            addEntity(objects.prefab.Entity)
        end
    end
    
end