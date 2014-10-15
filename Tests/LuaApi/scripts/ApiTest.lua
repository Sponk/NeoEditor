
dofile("luaunit.lua") 

-- Not perfect but good enough
function compare_float(float1, float2)
    return math.abs((float1-float2)/float2) < 0.00001
end

function compare_vec(vec1, vec2)
    local vec2choose = vec1
    local othervec = vec2
    if #vec1 > #vec2 then
        vec2choose = vec2
        othervec = vec1
    end
    
    for i = 1, #vec2choose, 1 do
        assertEquals(compare_float(vec2choose[i], othervec[i]), true)
    end
end

function testMeshLoading()
    local object = loadMesh("meshs/box.mesh")
    assertEquals(getName(object), "Entity0")
    assertEquals(loadMesh("DoesNotExist"), nil)    
end

function testLight()
    local object = createLight()
    assertEquals(getName(object), "Light0")
end

function testVec()
    local vector = vec3(15.3,18.5,19)
    compare_vec(vector, {15.3,18.5,19})

    vector = vec3(3,3,1) + vec3(5,5,9)
    compare_vec(vector, {8,8,10})
    
    vector = vec3(3,3,1) * vec3(5,5,9)
    compare_vec(vector, {15,15,9})
    
    vector = vec3(20,15,5) / vec3(5,5,5)
    compare_vec(vector, {4,3,1})
    
    assertEquals(compare_float(length({2,2,2}), 3.464101615), true)
    
    vector = normalize({15,17,20})
    assertEquals(length(vector), 1)
    
    assertEquals(compare_float(dot({2,5,1}, {7,3,4}), 33), true)
end

LuaUnit.run()
quit()