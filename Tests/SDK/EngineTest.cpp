//========================================================================
// Copyright (c) 2014-2015 Yannick Pflanzer <neo-engine.de>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#include <gtest/gtest.h>
#include <NeoEngine.h>
#include <Variable.h>
#include <cstring>

#include "TestSetup.h"

using namespace Neo;

class TestEngine: public TestNeoSDK
{};

TEST_F(TestEngine, ManualCollision_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	PhysicsContext* physics = engine->getPhysicsContext();
	Level* level = engine->getLevel();
	Scene* scene = level->getCurrentScene();

	OEntity* entity1 = scene->addNewEntity(NULL); 
	OEntity* entity2 = scene->addNewEntity(NULL);	

	entity1->setActive(true);
	entity1->setActive(true);

	entity1->setScale(Vector3(1,1,1));
	entity2->setScale(Vector3(1,1,1));

	Box3d* bbox = entity1->getBoundingBox();
	bbox->min = Vector3(-100,-100,-100);
	bbox->max = Vector3(100,100,100);

	bbox = entity2->getBoundingBox();
	bbox->min = Vector3(-100,-100,-100);
	bbox->max = Vector3(100,100,100);

	PhysicsProperties* phys1 = entity1->createPhysicsProperties();
	PhysicsProperties* phys2 = entity2->createPhysicsProperties();

	phys1->setGhost(true);
	phys2->setGhost(true);

	entity1->setPosition(Vector3(0,0,0));
	entity2->setPosition(Vector3(-99,-100,-100));

	entity1->updateMatrix();
	entity2->updateMatrix();

	scene->prepareCollisionShape(entity1);
	scene->prepareCollisionShape(entity2);

	scene->prepareCollisionObject(entity1);
	scene->prepareCollisionObject(entity2);

	//scene->preparePhysics();

	ASSERT_NE(0, phys1->getCollisionObjectId());
	ASSERT_NE(0, phys2->getCollisionObjectId());

	scene->update();
	scene->updatePhysics();

	EXPECT_EQ(true, physics->isObjectsCollision(phys1->getCollisionObjectId(),
												phys2->getCollisionObjectId()));
}

TEST_F(TestEngine, Collision_test)
{
	NeoEngine* engine = NeoEngine::getInstance();
	PhysicsContext* physics = engine->getPhysicsContext();
	Level* level = engine->getLevel();
	Scene* scene = level->getCurrentScene();

	// Prepare the entity
	OEntity* entity1 = scene->addNewEntity(NULL); 
	OEntity* entity2 = scene->addNewEntity(NULL);	

	entity1->setActive(true);
	entity1->setActive(true);

	entity1->setScale(Vector3(1,1,1));
	entity2->setScale(Vector3(1,1,1));

	Box3d* bbox = entity1->getBoundingBox();
	bbox->min = Vector3(-100,-100,-100);
	bbox->max = Vector3(100,100,100);

	bbox = entity2->getBoundingBox();
	bbox->min = Vector3(-100,-100,-100);
	bbox->max = Vector3(100,100,100);

	// Create physics properties
	PhysicsProperties* phys1 = entity1->createPhysicsProperties();
	PhysicsProperties* phys2 = entity2->createPhysicsProperties();

	// Change some settings
	phys1->setGhost(true);
	phys2->setGhost(true);

	entity1->setPosition(Vector3(0,0,0));
	entity2->setPosition(Vector3(-99,-100,-100));

	entity1->updateMatrix();
	entity2->updateMatrix();

	// Enable physics!
	entity1->enablePhysics();
	entity2->enablePhysics();

	// Check if it worked
	ASSERT_NE(0, phys1->getCollisionObjectId());
	ASSERT_NE(0, phys2->getCollisionObjectId());

	// Update!
	scene->update();
	scene->updatePhysics();

	EXPECT_EQ(true, entity1->isColliding(entity2));
}

// FIXME: Move to own file!
#include <Input.h>
#include <InputField.h>

TEST_F(TestEngine, TextInputTest)
{
	// Needs '|' because we skip the actual selection process
	const char* labelbefore = "LABEL|";
	const char* labelafter = "LABEL!|";

	Input input;
	NeoEngine* engine = NeoEngine::getInstance();
	engine->setInputContext(&input);

	Neo2D::Gui::InputField field(0,0,0,0,labelbefore);

	// Simulate key press!
	field.setState(Neo2D::Gui::INPUT_STATE::INPUT_SELECTED_STATE);
	input.setLastChar('!');
	field.update();
	
	ASSERT_EQ('\0', input.popLastChar());
	ASSERT_EQ(string(labelafter), string(field.getLabel()));
	engine->setInputContext(NULL);
}