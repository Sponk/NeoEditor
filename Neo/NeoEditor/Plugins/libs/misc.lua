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
