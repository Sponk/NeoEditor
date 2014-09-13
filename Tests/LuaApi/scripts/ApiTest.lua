
dofile("luaunit.lua") 

function testMeshLoading()
    local object = loadMesh("meshs/box.mesh")
    assertEquals(getName(object), "Entity0")
    assertEquals(loadMesh("DoesNotExist"), nil)    
end

function testLight()
    local object = createLight()
    assertEquals(getName(object), "Light0")
end

LuaUnit.run()
quit()