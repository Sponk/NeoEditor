-- Functions for data conversion

function str2vec(str)
    return split(str, " ")
end

function str2bool(str)
    if string.upper(str) == "TRUE" then return true
    elseif string.upper(str) == "FALSE" then return false end
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