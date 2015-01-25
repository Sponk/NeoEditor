
function assertVectorEquals(vec1, vec2, margin)
  for i = 1, #vec1, 1 do
    assertAlmostEquals(vec1[i], vec2[i], margin)
  end
end

Test3DSDK = {}
Test2DSDK = {}

function Test3DSDK:TestLight()

  local light = OLight()
  
  -- Test constructor
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

  -- Test Light specifics
  light:setIntensity(5)
  assertEquals(light:getIntensity(), 5)
  
  light:setColor({1,1,1})
  assertVectorEquals(light:getColor(), {1,1,1}, 0.001)
  
  light:setRadius(15)
  assertEquals(light:getRadius(), 15)
  
  light:enableShadows()
  assertEquals(light:hasShadows(), true)
  
  light:disableShadows()
  assertEquals(light:hasShadows(), false)
  
  light:setShadowBlur(15)
  assertEquals(light:getShadowBlur(), 15)
  
  light:setShadowBias(3)
  assertEquals(light:getShadowBias(), 3)
  
  light:setSpotAngle(45)
  assertEquals(light:getSpotAngle(), 45)
  
  light:makeDirectional()
  assertEquals(light:getSpotAngle(), 0)
  
  light:makePointLight()
  assertEquals(light:getSpotAngle(), 180)
end

function Test3DSDK:TestEntity()
  
  local entity = OEntity("meshs/box.mesh")
  
  -- Check if constructor works
  assertNotNil(entity)
  assertEquals(entity:getPosition(), {0,0,0})  
  
  assertEquals(entity:getPath(), getWorkingDirectory() .. "/meshs/box.mesh")
  
end

function Test3DSDK:TestCamera()
  
  local camera = OCamera()
  
  -- Check if constructor works
  assertNotNil(camera)
  assertNotNil(camera.nativeObject)

  camera:setClearColor({0.5, 0.5, 0.5})    
  assertVectorEquals(camera:getClearColor(), {0.5,0.5,0.5},  0.0001)
  
  camera:setFov(70)
  assertEquals(camera:getFov(), 70)
  
  camera:setNearPlane(0.5)
  assertAlmostEquals(camera:getNearPlane(), 0.5, 0.0001)
  
  camera:setFarPlane(1000)
  assertAlmostEquals(camera:getFarPlane(), 1000, 0.0001)
  
  camera:setFogDistance(200)
  assertAlmostEquals(camera:getFogDistance(), 200, 0.0001)
  
  camera:enableFog(true)
  assertEquals(camera:hasFog(), true)
  
  camera:enableOrtho(true)
  assertEquals(camera:isOrtho(), true)
end

function Test2DSDK:TestWidget()
  
  local widget = Widget(0,0,10,10, "Text")
  
  -- Add a real widget since the Widget class is purely abstract
  widget.widget = Button(0,0,10,10,"Nothing", "")
  
  widget:setVisible(true)
  assertEquals(widget:isVisible(), true)
    
end