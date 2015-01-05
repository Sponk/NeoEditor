
function assertVectorEquals(vec1, vec2, margin)
  for i = 1, #vec1, 1 do
    assertAlmostEquals(vec1[i], vec2[i], margin)
  end
end

function TestLight()

  local light = OLight()  
  assertNotNil(light)
  assertEquals(light:getPosition(), {0,0,0})

  light:setPosition({1,0,0})
  assertEquals(light:getPosition(), {1,0,0})

  light:setPosition({0,0,0})
  assertEquals(light:getPosition(), {0,0,0})
  
  light:translate({1,0,0})
  assertEquals(light:getPosition(), {1,0,0})
  
  light:rotate({1,0,0}, 180)
  assertEquals(light:getRotation(), {180,0,0})
    
  -- Reset to origin
  light:setPosition({0,0,0})
    
  light:translate({0,0,1}, "local")
  assertVectorEquals(light:getPosition(), {0,0,-1}, 0.00001)

end