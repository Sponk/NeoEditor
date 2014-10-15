setPluginName("Prefabs")
setPluginAuthor("Yannick Pflanzer")
setPluginDescription("A plugin that allows you to load and save your object selection as a prefab.")

addEditorMenu("Load Prefab from file", "load_callback")
addEditorMenu("Save selection as Prefab", "save_callback")

require("plugins.libs.libxml")
require("plugins.libs.libconv")
        
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

sep = "/"

-- From penlib
function relpath(P,start)

    -- If path is not absolute, make it so.
    if P:sub(1, 1) ~= sep then
    	P = sep .. P
    end
    
    if start:sub(1, 1) ~= sep then
    	start = sep .. start
    end
   
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

function objectToXml(object, path, parent)

    local objectType = getObjectType(object)
    local output = "\n\n<" .. objectType .. " name=\"" .. getName(object) .. "\""
    
    if parent ~= nil then
	output = output .. " parent=\"" .. getName(parent) .. "\""
    end
   
    if objectType == "Entity" then
        output = output .. " file=\"" .. relpath(getMeshFilename(object), path)
        output = output .. "\">\n"
    
        local min = getBoundingMin(object)
        local max = getBoundingMax(object)
    
        output = output .. "<BoundingBox min=\"" .. vec2str(min) .. "\" max=\"" .. vec2str(max) .. "\"/>\n"    
    
        -- Should be not isVisible!
        output = output .. "<ObjectProperties invisible=\"" .. bool2str(not isVisible(object)) .. "\"/>\n"
    
        -- TODO: Test for physics not for mass!
        if getMass(object) ~= nil then
            output = output .. "<physics\n"
            
            -- TODO: Get real shape!
            output = output .. "\tshape=\"Box\"\n"
        
            output = output .. "\tghost=\"" .. bool2str(isGhost(object)) .. "\"\n"        
            output = output .. "\tmass=\"" .. getMass(object) .. "\"\n"
            output = output .. "\tfriction=\"" .. getFriction(object) .. "\"\n"
            output = output .. "\trestitution=\"" .. getRestitution(object) .. "\"\n"
            output = output .. "\tlinearDamping=\"" .. getLinearDamping(object) .. "\"\n"
            output = output .. "\tangularDamping=\"" .. getAngularDamping(object) .. "\"\n"
            output = output .. "\tangularFactor=\"" .. getAngularFactor(object) .. "\"\n"
            output = output .. "\tlinearFactor=\"" .. vec2str(getLinearFactor(object)) .. "\"\n"
        
            output = output .. "/>\n"
        end
    elseif objectType == "Light" then
        output = output .. ">\n"
        
        output = output .. "<ObjectProperties\n"
        output = output .. "\tradius=\"" .. getLightRadius(object) .. "\"\n"
        output = output .. "\tcolor=\"" .. vec2str(getLightColor(object)) .. "\"\n"
        output = output .. "\tintensity=\"" .. getLightIntensity(object) .. "\"\n"
        output = output .. "\tspotAngle=\"" .. getLightSpotAngle(object) .. "\"\n"
        output = output .. "\tspotExponent=\"" .. getLightSpotExponent(object) .. "\"\n"
        output = output .. "\tshadow=\"" .. bool2str(isCastingShadow(object)) .. "\"\n"
    
        if isCastingShadow(object) then
            output = output .. "\tshadowBias=\"" .. getLightShadowBias(object) .. "\"\n"
            output = output .. "\tshadowBlur=\"" .. getLightShadowBlur(object) .. "\"\n"
            output = output .. "\tshadowQuality=\"" .. getLightShadowQuality(object) .. "\"\n"
        end
    
        output = output .. "/>\n"
    
    elseif objectType == "Camera" then
        output = output .. ">\n"
        output = output .. "<ObjectProperties\n"
        output = output .. "\tclearColor=\"" .. vec2str(getCameraClearColor(object)) .. "\"\n"
        output = output .. "\tortho=\"" .. bool2str(isCameraOrtho(object)) .. "\"\n"
        output = output .. "\tfov=\"" .. getCameraFov(object) .. "\"\n"
        output = output .. "\tclippingNear=\"" .. getCameraNear(object) .. "\"\n"
        output = output .. "\tclippingFar=\"" .. getCameraFar(object) .. "\"\n"
        output = output .. "\tfog=\"" .. bool2str(isCameraFogEnabled(object)) .. "\"\n"
        output = output .. "\tfogDistance=\"" .. getCameraFogDistance(object) .. "\"\n"
    
        output = output .. "/>\n"
    elseif objectType == "Sound" then
        output = output .. " file=\"" .. relpath(getSoundFilename(object), path)
        output = output .. "\">\n"
    
        output = output .. "<ObjectProperties\n"
        output = output .. "\tpitch=\"" .. getSoundPitch(object) .. "\"\n"
        output = output .. "\tgain=\"" .. getSoundGain(object) .. "\"\n"
        output = output .. "\trolloff=\"" .. getSoundRolloff(object) .. "\"\n"
        output = output .. "\tradius=\"" .. getSoundRadius(object) .. "\"\n"
        output = output .. "\trelative=\"" .. bool2str(isSoundRelative(object)) .. "\"\n"
        output = output .. "\tloop=\"" .. bool2str(isSoundLooping(object)) .. "\"\n"

    
        output = output .. "/>\n"
    elseif objectType == "Text" then
        output = output .. " file=\"" .. relpath(getFontFilename(object), path)
        output = output .. "\">\n"
    
        output = output .. "<ObjectProperties\n"
        output = output .. "\tsize=\"" .. getTextFontSize(object) .. "\"\n"
        output = output .. "\talign=\"" .. getTextAlignment(object) .. "\"\n"
        output = output .. "\tcolor=\"" .. vec2str(getTextColor(object)) .. "\"\n"
    
        -- FIXME: Possibly unsafe when using escape sequences!
        output = output .. "\ttext=\"" .. getText(object) .. "\"\n"
        output = output .. "/>\n"   
    elseif objectType == "Object" then
        messagebox("Your selection contains an unsupported object type. Please deselect all groups you want to save.")
        -- TODO: Stop this process here!
        return "";
    else
            output = output .. "\">\n"
    end
    
    -- Position is relative to the selection center
    local position = (getTransformedPosition(object) - getSelectionCenter())
    local rotation = getTransformedRotation(object)
    local scale = getTransformedScale(object)
    
    output = output .. "<transform\n"
    output = output .. "\tposition=\"" .. vec2str(position) .. "\"\n"
    output = output .. "\trotation=\"" .. vec2str(rotation) .. "\"\n"
    output = output .. "\tscale=\"" .. vec2str(scale) .. "\"\n"
    output = output .. "/>\n"

    output = output .. "<active value=\"" .. bool2str(isActive(object)) .. "\"/>\n"

    -- Output behaviors
    for i = 1, getBehaviorsNumber(object), 1 do
        local name = getBehaviorName(object, i)
        output = output .. "<behavior name=\"" .. name .. "\">\n"
        
        for j = 1, getBehaviorVariablesNumber(object, i), 1 do
            output = output .. "<BehaviorProperty\n"
            output = output .. "\tid=\"" .. j-1 .. "\"\n"
            
            variable = getBehaviorVariable(object,  i-1, j-1)
            if type(variable) == "table" then
                output = output .. "\tdata=\"" .. vec2str(variable) .. "\"\n"
            elseif type(variable) == "boolean" then
                output = output .. "\tdata=\"" .. bool2str(variable) .. "\"\n"
            else
                output = output .. "\tdata=\"" .. variable .. "\"\n"
            end

            output = output .. "\ttype=\"" .. getBehaviorVariableType(object, i, j) .. "\"\n"
            output = output .. "/>\n"
        end
        output = output .. "</behavior>\n"
    end

    output = output .. "</" .. objectType .. ">"

    return output
end

function convertWithChildren(object, path, parent)
	local ret = objectToXml(object, path, parent)
	local children = getChilds(object)
	
	for i = 1, #children, 1 do
		ret = ret .. convertWithChildren(children[i], path, object)
	end
	
	return ret
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

    local projectDir = getProjectDir()
    local filename = saveFileDlg("Choose a file", getProjectDir(), "*.mp");
    
    if filename == nil then return end
    
    -- Warn for out of project saves
    if string.sub(filename, 1, string.len(getProjectDir())) ~= projectDir then
        messagebox("WARNING: You try to save a prefab outside the project directory. This will probably not work!")
    end
    
    -- Ensure file ending
    if string.sub(filename, -string.len(".mp")) ~= ".mp" then
        filename = filename .. ".mp"
    end
    
    local path = getProjectDir()
    local content = "<prefab>"
       
    for i = 1, #selection, 1 do
	-- Only add object when it does not already exist. We are checking for the name here
	if string.find(content, "name=\"" .. getName(selection[i]) .. "\"") == nil then
		content = content .. convertWithChildren(selection[i], path)
	end
    end
    
    content = content .. "</prefab>"
    
    -- print(content)
    
    local output = io.open(filename, "w")
    output:write(content)
    output:close()
end

function updateTransform(settings, object)  
    setPosition(object, str2vec(settings.transform["@position"]))
    setScale(object, str2vec(settings.transform["@scale"]))
    setRotation(object, str2vec(settings.transform["@rotation"]))
    
    if str2bool(settings.active["@value"]) then
        activate(object)
    else
        deactivate(object)
    end
    
    updateMatrix(object)
end

function addProperty(settings, object)
        addBehavior(object, settings["@name"])
        local idx = getBehaviorsNumber(object) - 1
        
        properties = settings.BehaviorProperty
        
        -- Properties
        if properties ~= nil then
            if #properties > 0 then  
                for i = 1, #properties, 1 do
                    local data
                    
                    if string.find(getBehaviorVariableType(object, idx+1, i), "vec") then
                        data = str2vec(properties[i]["@data"])
                    else
                        data = properties[i]["@data"]
                    end
            
                    setBehaviorVariable(object, idx, tonumber(properties[i]["@id"]), data)
                end
            else
                    setBehaviorVariable(object, idx, tonumber(properties["@id"]), properties["@data"])
            end
        end
end

function loadBehaviors(settings, object)

    if settings == nil then return end
    
    if #settings == 0 then
        addProperty(settings, object)
    else
        for j = 1, #settings, 1 do
            addProperty(settings[j], object)
        end
    end  
    
end

function addEntity(entity, group)
    local object = loadMesh(entity["@file"])
    
    loadBehaviors(entity.behavior, object)
    updateTransform(entity, object)    
    setInvisible(object, str2bool(entity.ObjectProperties["@invisible"]))   
   
    local physics = entity.physics
    if physics ~= nil then
        setMass(object, physics["@mass"])
        setFriction(object, physics["@friction"])
        setLinearDamping(object, physics["@linearDamping"])
        setAngularDamping(object, physics["@angularDamping"])
        setAngularFactor(object, physics["@angularFactor"])
        setLinearFactor(object, str2vec(physics["@linearFactor"]))
        enableGhost(object, str2bool(physics["@ghost"]))
    end
   
    if entity["@parent"] == nil then
        setParent(object, group)
    end  
    
    return object
end

function addLight(light, group)
    local object = createLight()
    updateTransform(light, object)
    setScale(object, {1,1,1})
    updateMatrix(object)
    
    local prop = light.ObjectProperties
    
    setLightColor(object, str2vec(prop["@color"]))
    setLightRadius(object, prop["@radius"])
    setLightIntensity(object, prop["@intensity"])
    setLightSpotAngle(object, prop["@spotAngle"])
    
    if str2bool(prop["@shadow"]) then
        enableShadow(object, true)
        setLightShadowBias(object, prop["@shadowBias"])
        setLightShadowBlur(object, prop["@shadowBlur"])
        setLightShadowQuality(object, prop["@shadowQuality"])
    end
    
    if light["@parent"] == nil then
        setParent(object, group)
    end
    
    return object
end

function addCamera(camera, group)
    local object = createCamera()
    updateTransform(camera, object)    
    setScale(object, {1,1,1})
    updateMatrix(object)
    
    local prop = camera.ObjectProperties   
    
    setCameraClearColor(object, str2vec(prop["@clearColor"]))
    enableCameraOrtho(object, str2bool(prop["@ortho"]))
    enableCameraFog(object, str2bool(prop["@fog"]))
    setCameraFogDistance(object, prop["@fogDistance"])
    setCameraFov(object, prop["@fov"])
    setCameraNear(object, prop["@clippingNear"])
    setCameraFar(object, prop["@clippingFar"])    
    
    if camera["@parent"] == nil then
        setParent(object, group)
    end
    
    return object
end

function addSound(sound, group)
    local object = loadSound(sound["@file"])
    updateTransform(sound, object)    
    setScale(object, {1,1,1})
    updateMatrix(object)
    
    local prop = sound.ObjectProperties   
    
    setSoundLooping(object, prop["@loop"])
    setSoundRelative(object, prop["@relative"])
    setSoundPitch(object, prop["@pitch"])
    setSoundGain(object, prop["@gain"])
    setSoundRadius(object, prop["@radius"])
    setSoundRolloff(object, prop["@rolloff"])
    
    if sound["@parent"] == nil then
        setParent(object, group)
    end
    
    return object
end

function addText(text, group)
    local object = loadTextFont(text["@file"])
    updateTransform(text, object)    
    setScale(object, {1,1,1})
    updateMatrix(object)
    
    local prop = text.ObjectProperties   
        
    setTextFontSize(object, prop["@size"])
    setTextColor(object, str2vec(prop["@color"]))
    setTextAlignment(object, prop["@align"])
    setText(object, prop["@text"])
        
    if text["@parent"] == nil then
        setParent(object, group)
    end
    
    return object
end

function addGroup(data, group)
    local object = createGroup()
    updateTransform(data, object)    
    setScale(object, {1,1,1})
    updateMatrix(object)
        
    if data["@parent"] == nil then
        setParent(object, group)
    end
    
    return object
end

function load_callback()

    local filename = openFileDlg("Choose a file", getProjectDir(), "*.mp")   
    
    if filename == nil then return end
        
    local xml = newParser()
    local f = io.open(filename, "r")
    local content = f:read("*all")
    f:close()

    local objects = xml:ParseXmlText(content)
    local group = createGroup()
    local parents = {}
    local allObjects = {}
    
    --print(getName(group))
    
    -- Entities
    if objects.prefab.Entity ~= nil then
        if #objects.prefab.Entity > 0 then  
            for i = 1, #objects.prefab.Entity, 1 do
		parents[objects.prefab.Entity[i]["@name"]] = addEntity(objects.prefab.Entity[i], group)
		allObjects[#allObjects + 1] = objects.prefab.Entity[i]
	    end
        else
		parents[objects.prefab.Entity["@name"]] = addEntity(objects.prefab.Entity, group)
		allObjects[#allObjects + 1] = objects.prefab.Entity
        end
    end
    
    -- Lights
    if objects.prefab.Light ~= nil then
        if #objects.prefab.Light > 0 then  
            for i = 1, #objects.prefab.Light, 1 do
		parents[objects.prefab.Light[i]["@name"]] = addLight(objects.prefab.Light[i], group)
		allObjects[#allObjects + 1] = objects.prefab.Light[i]
            end
        else
		parents[objects.prefab.Light["@name"]] = addLight(objects.prefab.Light, group)
		allObjects[#allObjects + 1] = objects.prefab.Light
        end
    end
    
    -- Cameras
    if objects.prefab.Camera ~= nil then
        if #objects.prefab.Camera > 0 then  
            for i = 1, #objects.prefab.Camera, 1 do
		parents[objects.prefab.Camera[i]["@name"]] = addCamera(objects.prefab.Camera[i], group)
		allObjects[#allObjects + 1] = objects.prefab.Camera[i]
            end
        else
		parents[objects.prefab.Camera["@name"]] = addCamera(objects.prefab.Camera, group)
		allObjects[#allObjects + 1] = objects.prefab.Camera
        end
    end
    
    -- Sounds
    if objects.prefab.Sound ~= nil then
        if #objects.prefab.Sound > 0 then  
            for i = 1, #objects.prefab.Sound, 1 do
		parents[objects.prefab.Sound[i]["@name"]] = addSound(objects.prefab.Sound[i], group)
		allObjects[#allObjects + 1] = objects.prefab.Sound[i]
            end
        else
		parents[objects.prefab.Sound["@name"]] = addSound(objects.prefab.Sound, group)
		allObjects[#allObjects + 1] = objects.prefab.Sound
        end
    end
    
    -- Texts
    if objects.prefab.Text ~= nil then
        if #objects.prefab.Text > 0 then  
            for i = 1, #objects.prefab.Text, 1 do
		parents[objects.prefab.Text[i]["@name"]] = addText(objects.prefab.Text[i], group)
		allObjects[#allObjects + 1] = objects.prefab.Text[i]
            end
        else
		parents[objects.prefab.Text["@name"]] = addText(objects.prefab.Text, group)
		allObjects[#allObjects + 1] = objects.prefab.Text
        end
    end
    
    -- Groups (objects)
    if objects.prefab.Object ~= nil then
        if #objects.prefab.Object > 0 then  
            for i = 1, #objects.prefab.Object, 1 do
		parents[objects.prefab.Object[i]["@name"]] = addGroup(objects.prefab.Object[i], group)
		allObjects[#allObjects + 1] = objects.prefab.Text[i]
            end
        else
		parents[objects.prefab.Object["@name"]] = addGroup(objects.prefab.Object, group)
		allObjects[#allObjects + 1] = objects.prefab.Object
        end
    end
    
    -- Set all parent-child relationships
    for i = 1, #allObjects, 1 do
	if allObjects[i]["@parent"] ~= nil then
		setParent(parents[allObjects[i]["@name"]], parents[allObjects[i]["@parent"]])
	end
    end
    
    allObjects = nil
    parents = nil
    updateEditorView()    
end

debugLog("Plugin loaded")
