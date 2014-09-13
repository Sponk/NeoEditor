
dofile("luaunit.lua") 

function testMultiplyZero()
    assertEquals(1*0,0)
    assertEquals(0*5,0)
    assertEquals(0*0,0)
end

LuaUnit.run()
quit()