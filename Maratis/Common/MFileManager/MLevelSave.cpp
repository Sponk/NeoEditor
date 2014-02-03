/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MLevelSave.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
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


#include <MEngine.h>
#include "MLevelSave.h"
#include "MXmlCommon.h"


static char rep[256];


void writePhysics(MFile * file, MPhysicsProperties * physicsProperties)
{
	openAttributeNode(file, "physics", 3);
	M_fprintf(file, "\n");

	// shape
	M_fprintf(file, "\t\t\t\t");
	switch(physicsProperties->getCollisionShape())
	{
	case M_COLLISION_SHAPE_BOX:
		writeString(file, "shape", "Box");
		break;
	case M_COLLISION_SHAPE_SPHERE:
		writeString(file, "shape", "Sphere");
		break;
	case M_COLLISION_SHAPE_CONE:
		writeString(file, "shape", "Cone");
		break;
	case M_COLLISION_SHAPE_CAPSULE:
		writeString(file, "shape", "Capsule");
		break;
	case M_COLLISION_SHAPE_CYLINDER:
		writeString(file, "shape", "Cylinder");
		break;
	case M_COLLISION_SHAPE_CONVEX_HULL:
		writeString(file, "shape", "ConvexHull");
		break;
	case M_COLLISION_SHAPE_TRIANGLE_MESH:
		writeString(file, "shape", "TriangleMesh");
		break;
	}
	M_fprintf(file, "\n");

	// ghost
	M_fprintf(file, "\t\t\t\t");
	writeBool(file, "ghost", physicsProperties->isGhost());
	M_fprintf(file, "\n");

	// mass
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "mass", physicsProperties->getMass());
	M_fprintf(file, "\n");

	// friction
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "friction", physicsProperties->getFriction());
	M_fprintf(file, "\n");

	// restitution
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "restitution", physicsProperties->getRestitution());
	M_fprintf(file, "\n");

	// linearDamping
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "linearDamping", physicsProperties->getLinearDamping());
	M_fprintf(file, "\n");

	// angularDamping
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "angularDamping", physicsProperties->getAngularDamping());
	M_fprintf(file, "\n");

	// angularFactor
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "angularFactor", physicsProperties->getAngularFactor());
	M_fprintf(file, "\n");

	// linearFactor
	M_fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "linearFactor", *physicsProperties->getLinearFactor(), 3);
	
	closeAttributeNode(file);
	M_fprintf(file, "\n");
	
	// constraint
	MPhysicsConstraint * constraint = physicsProperties->getConstraint();
	if(constraint)
	{
		openAttributeNode(file, "constraint", 3);
		M_fprintf(file, "\n");
		
		// parent
		M_fprintf(file, "\t\t\t\t");
		writeString(file, "parent", constraint->parentName.getSafeString());
		M_fprintf(file, "\n");
		
		// pivot
		M_fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "pivot", constraint->pivot, 3);
		M_fprintf(file, "\n");
		
		// lowerLinearLimit
		M_fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "lowerLinearLimit", constraint->lowerLinearLimit, 3);
		M_fprintf(file, "\n");
		
		// upperLinearLimit
		M_fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "upperLinearLimit", constraint->upperLinearLimit, 3);
		M_fprintf(file, "\n");
		
		// lowerAngularLimit
		M_fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "lowerAngularLimit", constraint->lowerAngularLimit, 3);
		M_fprintf(file, "\n");
		
		// upperAngularLimit
		M_fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "upperAngularLimit", constraint->upperAngularLimit, 3);
		M_fprintf(file, "\n");
		
		// disableParentCollision
		M_fprintf(file, "\t\t\t\t");
		writeBool(file, "disableParentCollision", constraint->disableParentCollision);
		
		closeAttributeNode(file);
		M_fprintf(file, "\n");
	}
}

void writeCameraProperties(MFile * file, MOCamera * camera)
{
	openAttributeNode(file, "properties", 3);
	M_fprintf(file, "\n");

	// clearColor
	M_fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "clearColor", camera->getClearColor(), 3);
	M_fprintf(file, "\n");

	// ortho
	M_fprintf(file, "\t\t\t\t");
	writeBool(file, "ortho", camera->isOrtho());
	M_fprintf(file, "\n");

	// fov
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "fov", camera->getFov());
	M_fprintf(file, "\n");

	// clippingNear
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "clippingNear", camera->getClippingNear());
	M_fprintf(file, "\n");

	// clippingFar
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "clippingFar", camera->getClippingFar());
	M_fprintf(file, "\n");

	// fog
	M_fprintf(file, "\t\t\t\t");
	writeBool(file, "fog", camera->hasFog());
	M_fprintf(file, "\n");

	// fogDistance
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "fogDistance", camera->getFogDistance());

	closeAttributeNode(file);
	M_fprintf(file, "\n");
}

void writeEntityProperties(MFile * file, MOEntity * entity)
{
	openAttributeNode(file, "properties", 3);
	M_fprintf(file, "\n");

	// invisible
	M_fprintf(file, "\t\t\t\t");
	writeBool(file, "invisible", entity->isInvisible());

	closeAttributeNode(file);
	M_fprintf(file, "\n");
}

void writeSoundProperties(MFile * file, MOSound * sound)
{
	openAttributeNode(file, "properties", 3);
	M_fprintf(file, "\n");

	// loop
	M_fprintf(file, "\t\t\t\t");
	writeBool(file, "loop", sound->isLooping());
	M_fprintf(file, "\n");

	// pitch
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "pitch", sound->getPitch());
	M_fprintf(file, "\n");

	// gain
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "gain", sound->getGain());
	M_fprintf(file, "\n");

	// radius
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "radius", sound->getRadius());
	M_fprintf(file, "\n");

	// rolloff
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "rolloff", sound->getRolloff());
	M_fprintf(file, "\n");
	
	// relative
	M_fprintf(file, "\t\t\t\t");
	writeBool(file, "relative", sound->isRelative());
	
	closeAttributeNode(file);
	M_fprintf(file, "\n");
}

void writeTextProperties(MFile * file, MOText * text)
{
	openAttributeNode(file, "properties", 3);
	M_fprintf(file, "\n");

	// size
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "size", text->getSize());
	M_fprintf(file, "\n");

	// align
	M_fprintf(file, "\t\t\t\t");
	switch(text->getAlign())
	{
	case M_ALIGN_LEFT:
		writeString(file, "align", "Left");
		break;
	case M_ALIGN_RIGHT:
		writeString(file, "align", "Right");
		break;
	case M_ALIGN_CENTER:
		writeString(file, "align", "Center");
		break;
	}
	M_fprintf(file, "\n");

	// color
	M_fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "color", text->getColor(), 4);

	closeAttributeNode(file);
	M_fprintf(file, "\n");

	// text
	if(text->getText())
	{
		openNode(file, "textData", 3);
		M_fprintf(file, "\n");

		M_fprintf(file, "\t\t\t\t<![CDATA[");
		M_fprintf(file, "%s", text->getText());
		M_fprintf(file, "]]>");
		M_fprintf(file, "\n");

		closeNode(file, "textData", 3);
		M_fprintf(file, "\n");
	}
}

void writeSceneProperties(MFile * file, MScene * scene)
{
	openAttributeNode(file, "properties", 2);
	M_fprintf(file, "\n");

	// data mode
	M_DATA_MODES dataMode = scene->getDataMode();
	switch(dataMode)
	{
	case M_DATA_STATIC:
		M_fprintf(file, "\t\t\t");
		writeString(file, "data", "Static");
		M_fprintf(file, "\n");
		break;
	case M_DATA_DYNAMIC:
		M_fprintf(file, "\t\t\t");
		writeString(file, "data", "Dynamic");
		M_fprintf(file, "\n");
		break;
	case M_DATA_STREAM:
		M_fprintf(file, "\t\t\t");
		writeString(file, "data", "Stream");
		M_fprintf(file, "\n");
		break;
	}

	// gravity
	M_fprintf(file, "\t\t\t");
	writeFloatValues(file, "gravity", scene->getGravity(), 3);

	closeAttributeNode(file);
	M_fprintf(file, "\n");
}

void writeLightProperties(MFile * file, MOLight * light)
{
	openAttributeNode(file, "properties", 3);
	M_fprintf(file, "\n");

	// radius
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "radius", light->getRadius());
	M_fprintf(file, "\n");

	// color
	M_fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "color", light->getColor(), 3);
	M_fprintf(file, "\n");

	// intensity
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "intensity", light->getIntensity());
	M_fprintf(file, "\n");

	// spotAngle
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "spotAngle", light->getSpotAngle());
	M_fprintf(file, "\n");

	// spotExponent
	M_fprintf(file, "\t\t\t\t");
	writeFloat(file, "spotExponent", light->getSpotExponent());
	M_fprintf(file, "\n");
	
	// shadow
	M_fprintf(file, "\t\t\t\t");
	writeBool(file, "shadow", light->isCastingShadow());
	
	if(light->isCastingShadow())
	{
		M_fprintf(file, "\n");
		
		// shadow bias
		M_fprintf(file, "\t\t\t\t");
		writeFloat(file, "shadowBias", light->getShadowBias());
		M_fprintf(file, "\n");
	
		// shadow blur
		M_fprintf(file, "\t\t\t\t");
		writeFloat(file, "shadowBlur", light->getShadowBlur());
		M_fprintf(file, "\n");
	
		// shadow quality
		M_fprintf(file, "\t\t\t\t");
		writeInt(file, "shadowQuality", light->getShadowQuality());
	}
	
	closeAttributeNode(file);
	M_fprintf(file, "\n");
}

void writeObjectTransform(MFile * file, MObject3d * object)
{
	openAttributeNode(file, "active", 3);
	writeBool(file, "value", object->isActive());
	closeAttributeNode(file);
	M_fprintf(file, "\n");

	openAttributeNode(file, "transform", 3);
	M_fprintf(file, "\n");

	// parent
	MObject3d * parent = object->getParent();
	if(parent)
	{
		M_fprintf(file, "\t\t\t\t");
		writeString(file, "parent", parent->getName());
		M_fprintf(file, "\n");
	}

	// position
	M_fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "position", object->getPosition(), 3);
	M_fprintf(file, "\n");

	// rotation
	M_fprintf(file, "\t\t\t\t");
	MVector3 rotation = object->getEulerRotation();
	writeFloatValues(file, "rotation", rotation, 3);
	M_fprintf(file, "\n");

	// scale
	M_fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "scale", object->getScale(), 3);

	closeAttributeNode(file);
	M_fprintf(file, "\n");
}

void writeBehavior(MFile * file, MBehavior * behavior)
{
	openAttributeNode(file, "Behavior", 3);
	writeString(file, "name", behavior->getName());
	M_fprintf(file, ">\n");

	openAttributeNode(file, "properties", 4);
	M_fprintf(file, "\n");

	unsigned int i;
	unsigned int size = behavior->getVariablesNumber();
	for(i=0; i<size; i++)
	{
		MVariable variable = behavior->getVariable(i);

		M_VARIABLE_TYPE varType = variable.getType();
		if(varType == M_VARIABLE_NULL)
			continue;

		M_fprintf(file, "\t\t\t\t\t");
        writeVariable(file, &variable, rep);

		if((i+1) < size)
			M_fprintf(file, "\n");
	}

	closeAttributeNode(file);
	M_fprintf(file, "\n");

	closeNode(file, "Behavior", 3);
	M_fprintf(file, "\n");
}

void writeBehaviors(MFile * file, MObject3d * object)
{
	unsigned int i;
	unsigned int size = object->getBehaviorsNumber();
	for(i=0; i<size; i++)
	{
		MBehavior * behavior = object->getBehavior(i);
		writeBehavior(file, behavior);
	}
}

bool xmlLevelSave(MLevel * level, const char * filename)
{
	MFile * file = M_fopen(filename, "wt");
	if(! file)
		return false;

	char localFile[256];

	// get rep
	getRepertory(rep, filename);

	openNode(file, "Maratis version=\"3.0\"", 0);
	M_fprintf(file, "\n\n");
	openNode(file, "Level", 0);
	M_fprintf(file, "\n\n");

	openAttributeNode(file, "properties", 1);
	writeInt(file, "currentScene", level->getCurrentSceneId());
	closeAttributeNode(file);
	M_fprintf(file, "\n\n");

	// scenes
	unsigned int i;
	unsigned int scnSize = level->getScenesNumber();
	for(i=0; i<scnSize; i++)
	{
		MScene * scene = level->getSceneByIndex(i);

		openAttributeNode(file, "Scene", 1);
		writeString(file, "name", scene->getName());
		M_fprintf(file, ">\n\n");
		
		// script
		if(scene->getScriptFilename())
		{
			if(strlen(scene->getScriptFilename()) > 0)
			{
				getLocalFilename(localFile, rep, scene->getScriptFilename());

				openAttributeNode(file, "script", 2);
				writeString(file, "file", localFile);
				closeAttributeNode(file);
				M_fprintf(file, "\n");
			}
		}

		// properties
		writeSceneProperties(file, scene);
		M_fprintf(file, "\n");

		// lights
		unsigned int l;
		unsigned int lSize = scene->getLightsNumber();
		for(l=0; l<lSize; l++)
		{
			MOLight * light = scene->getLightByIndex(l);

			openAttributeNode(file, "Light", 2);
			writeString(file, "name", light->getName());
			M_fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, light);

			// properties
			writeLightProperties(file, light);

			// behaviors
			writeBehaviors(file, light);

			closeNode(file, "Light", 2);
			M_fprintf(file, "\n");
		}

		// cameras
		unsigned int c;
		unsigned int cSize = scene->getCamerasNumber();
		for(c=0; c<cSize; c++)
		{
			MOCamera * camera = scene->getCameraByIndex(c);

			openAttributeNode(file, "Camera", 2);
			writeString(file, "name", camera->getName());
			M_fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, camera);

			// properties
			writeCameraProperties(file, camera);

			// behaviors
			writeBehaviors(file, camera);

			closeNode(file, "Camera", 2);
			M_fprintf(file, "\n");
		}

		// sound
		unsigned int s;
		unsigned int sSize = scene->getSoundsNumber();
		for(s=0; s<sSize; s++)
		{
			MOSound * sound = scene->getSoundByIndex(s);

			openAttributeNode(file, "Sound", 2);

			// name
			writeString(file, "name", sound->getName());

			// file
			const char * soundFile = NULL;

			// sound ref
			MSoundRef * ref = sound->getSoundRef();
			if(ref)
				soundFile = ref->getFilename();

			if(soundFile)
			{
				getLocalFilename(localFile, rep, soundFile);
				M_fprintf(file, " ");
				writeString(file, "file", localFile);
			}

			M_fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, sound);

			// properties
			writeSoundProperties(file, sound);

			// behaviors
			writeBehaviors(file, sound);

			closeNode(file, "Sound", 2);
			M_fprintf(file, "\n");
		}

		// entities
		unsigned int e;
		unsigned int eSize = scene->getEntitiesNumber();
		for(e=0; e<eSize; e++)
		{
			MOEntity * entity = scene->getEntityByIndex(e);

			openAttributeNode(file, "Entity", 2);

			// name
			writeString(file, "name", entity->getName());

			// file
			MMeshRef * ref = entity->getMeshRef();
			if(ref)
			{
				const char * meshFile = ref->getFilename();
				if(meshFile)
				{
					getLocalFilename(localFile, rep, meshFile);
					M_fprintf(file, " ");
					writeString(file, "file", localFile);
				}
			}

			M_fprintf(file, ">\n");

			// bounding box
			openAttributeNode(file, "BoundingBox", 3);
			writeFloatValues(file, "min", entity->getBoundingBox()->min, 3);
			M_fprintf(file, " ");
			writeFloatValues(file, "max", entity->getBoundingBox()->max, 3);
			closeAttributeNode(file);
			M_fprintf(file, "\n");

			// anim
			openAttributeNode(file, "anim", 3);
			writeInt(file, "id", (int)entity->getAnimationId());
			closeAttributeNode(file);
			M_fprintf(file, "\n");

			// transform
			writeObjectTransform(file, entity);

			// properties
			writeEntityProperties(file, entity);

			// physics
			MPhysicsProperties * physicsProperties = entity->getPhysicsProperties();
			if(physicsProperties)
				writePhysics(file, physicsProperties);

			// behaviors
			writeBehaviors(file, entity);

			closeNode(file, "Entity", 2);
			M_fprintf(file, "\n");
		}

		// text
		unsigned int t;
		unsigned int tSize = scene->getTextsNumber();
		for(t=0; t<tSize; t++)
		{
			MOText * text = scene->getTextByIndex(t);

			openAttributeNode(file, "Text", 2);

			// name
			writeString(file, "name", text->getName());

			// font
			MFont * font = text->getFont();
			if(font)
			{
				MFontRef * ref = text->getFontRef();
				const char * fontFile = ref->getFilename();
				if(fontFile)
				{
					getLocalFilename(localFile, rep, fontFile);
					M_fprintf(file, " ");
					writeString(file, "file", localFile);
				}
			}

			M_fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, text);

			// properties
			writeTextProperties(file, text);

			// behaviors
			writeBehaviors(file, text);

			closeNode(file, "Text", 2);
			M_fprintf(file, "\n");
		}

		M_fprintf(file, "\n");
		closeNode(file, "Scene", 1);
		M_fprintf(file, "\n");
	}


	M_fprintf(file, "\n");
	closeNode(file, "Level", 0);
	M_fprintf(file, "\n\n");
	closeNode(file, "Maratis", 0);

	M_fclose(file);
	return true;
}