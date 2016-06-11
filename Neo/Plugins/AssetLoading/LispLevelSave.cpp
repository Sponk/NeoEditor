//========================================================================
// Copyright (c) 2014-2016 Yannick Pflanzer <www.neo-engine.de>
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

#include <NeoEngine.h>

using namespace Neo;
#define LEVEL_VERSION_STRING "0.1"

static char g_rep[256];

void writeTabs(File* f, int num)
{
	for(int i = 0; i < num; i++)
		M_fprintf(f, "\t");
}

void writeStringVar(File* f, const char* name, const char* str, int tabs = 0)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(%s \"%s\")\n", name, str);
}

void writeFloatVar(File* f, const char* name, float v, int tabs = 0)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(%s %f)\n", name, v);
}

void writeIntegerVar(File* f, const char* name, int v, int tabs = 0)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(%s %d)\n", name, v);
}

void writeVector2(File* f, const char* name, const Vector2& v, int tabs = 0)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(%s %f %f)\n", name, v.x, v.y);
}

void writeVector3(File* f, const char* name, const Vector3& v, int tabs = 0)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(%s %f %f %f)\n", name, v.x, v.y, v.z);
}

void writeVector4(File* f, const char* name, const Vector4& v, int tabs = 0)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(%s %f %f %f %f)\n", name, v.x, v.y, v.z, v.w);
}

int findTextureId(Texture* tex, Mesh* mesh)
{
	for(int i = 0; i < mesh->getTexturesNumber(); i++)
		if(mesh->getTexture(i) == tex)
			return i;

	return -1;
}

std::string getLocalFile(const char* name)
{
	if(!name)
		return "(null)";

	char filename[256];
	getLocalFilename(filename, g_rep, name);
	return std::string(filename);
}

void writeTransform(File* f, Object3d* o, int tabs)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(transform\n");
	tabs++;

	writeVector3(f, "position", o->getPosition(), tabs);
	writeVector3(f, "rotation", o->getEulerRotation(), tabs);
	writeVector3(f, "scale", o->getScale(), tabs);

	tabs--;
	writeTabs(f, tabs);
	M_fprintf(f, ")\n");
}

void writeLight(File* f, OLight* l, int tabs)
{
	writeStringVar(f, "type", "Light", tabs);
	writeVector3(f, "color", l->getColor(), tabs);
	writeFloatVar(f, "radius", l->getRadius(), tabs);
	writeFloatVar(f, "intensity", l->getIntensity(), tabs);
	writeFloatVar(f, "spot-angle", l->getSpotAngle(), tabs);
	writeFloatVar(f, "spot-exponent", l->getSpotExponent(), tabs);
	writeIntegerVar(f, "has-shadow", l->isCastingShadow(), tabs);
	writeFloatVar(f, "shadow-bias", l->getShadowBias(), tabs);
	writeFloatVar(f, "shadow-blur", l->getShadowBlur(), tabs);
	writeIntegerVar(f, "shadow-quality", l->getShadowQuality(), tabs);
}

void writeCamera(File* f, OCamera* l, int tabs)
{
	writeStringVar(f, "type", "Camera", tabs);
	writeIntegerVar(f, "has-fog", l->hasFog(), tabs);
	writeFloatVar(f, "fog-distance", l->getFogDistance(), tabs);
	writeVector3(f, "fog-color", l->getFogColor(), tabs);
	writeVector3(f, "clear-color", l->getClearColor(), tabs);
	writeFloatVar(f, "clipping-near", l->getClippingNear(), tabs);
	writeFloatVar(f, "clipping-far", l->getClippingFar(), tabs);
	writeFloatVar(f, "fov", l->getFov(), tabs);
	writeIntegerVar(f, "is-ortho", l->isOrtho(), tabs);
}

void writeText(File* f, OText* t, int tabs)
{
	writeStringVar(f, "type", "Text", tabs);
	writeStringVar(f, "text", t->getText(), tabs);
	writeVector4(f, "color", t->getColor(), tabs);
	writeIntegerVar(f, "alignment", t->getAlign(), tabs);
	writeFloatVar(f, "size", t->getSize(), tabs);
	writeStringVar(f, "font", getLocalFile(t->getFontRef()->getFilename()).c_str(), tabs);
}

void writeSound(File* f, OSound* t, int tabs)
{
	writeStringVar(f, "type", "Sound", tabs);
	writeStringVar(f, "sound", getLocalFile(t->getSoundRef()->getFilename()).c_str(), tabs);
	writeFloatVar(f, "pitch", t->getPitch(), tabs);
	writeFloatVar(f, "gain", t->getGain(), tabs);
	writeFloatVar(f, "radius", t->getRadius(), tabs);
	writeFloatVar(f, "rolloff", t->getRolloff(), tabs);
	writeIntegerVar(f, "is-looping", t->isLooping(), tabs);
	writeIntegerVar(f, "is-relative", t->isRelative(), tabs);
}

void writePhysicsProperties(File* f, PhysicsProperties* p, int tabs)
{
	if(!p)
		return;

	writeTabs(f, tabs);
	M_fprintf(f, "(physics-properties\n");

	tabs++;
	writeIntegerVar(f, "is-ghost", p->isGhost(), tabs);
	writeFloatVar(f, "mass", p->getMass(), tabs);
	writeFloatVar(f, "friction", p->getFriction(), tabs);
	writeFloatVar(f, "restitution", p->getRestitution(), tabs);
	writeFloatVar(f, "linear-damping", p->getLinearDamping(), tabs);
	writeFloatVar(f, "angular-damping", p->getAngularDamping(), tabs);
	writeFloatVar(f, "angular-factor", p->getAngularFactor(), tabs);
	writeVector3(f, "linear-factor", *p->getLinearFactor(), tabs);
	writeIntegerVar(f, "collision-shape", p->getCollisionShape(), tabs);

	auto constraints = p->getConstraint();
	if(constraints)
	{
		writeTabs(f, tabs);
		M_fprintf(f, "(physics-constraint\n");
		tabs++;

		writeStringVar(f, "parent-name", constraints->parentName.getSafeString(), tabs);
		writeVector3(f, "pivot", constraints->pivot, tabs);
		writeVector3(f, "lower-linear-limit", constraints->lowerLinearLimit, tabs);
		writeVector3(f, "lower-angular-limit", constraints->lowerAngularLimit, tabs);
		writeVector3(f, "upper-linear-limit", constraints->upperLinearLimit, tabs);
		writeVector3(f, "upper-angular-limit", constraints->upperAngularLimit, tabs);
		writeIntegerVar(f, "disable-parent-collision", constraints->disableParentCollision, tabs);

		tabs--;
		writeTabs(f, tabs);
		M_fprintf(f, ")\n");
	}

	tabs--;

	writeTabs(f, tabs);
	M_fprintf(f, ")\n");

	// TODO: Write constraints
}

void writeEntity(File* f, OEntity* e, int tabs)
{
	writeStringVar(f, "type", "Entity", tabs);
	writeIntegerVar(f, "invisible", e->isInvisible(), tabs);
	writeStringVar(f, "mesh", getLocalFile(e->getMeshRef()->getFilename()).c_str(), tabs);
	writeIntegerVar(f, "has-shadow", e->hasShadow(), tabs);

	// Physics
	writePhysicsProperties(f, e->getPhysicsProperties(), tabs);

	// Textures
	{
        writeIntegerVar(f, "texture-count", e->getTexturesNumber(), tabs);
        writeTabs(f, tabs);
        size_t texCount = 0;
        M_fprintf(f, "(textures\n");
		tabs++;

        for(int j = 0; j < e->getTexturesNumber(); j++)
		{
            Texture* t = e->getTexture(j);

			writeTabs(f, tabs);
			M_fprintf(f, "(texture\n");
			tabs++;

			writeIntegerVar(f, "gen-mode", t->getGenMode(), tabs);
			writeIntegerVar(f, "uw-wrap", t->getUWrapMode(), tabs);
			writeIntegerVar(f, "vw-wrap", t->getVWrapMode(), tabs);
			writeVector2(f, "tex-translate", t->getTexTranslate(), tabs);
			writeFloatVar(f, "tex-rotate", t->getTexRotate(), tabs);
			writeVector2(f, "tex-scale", t->getTexScale(), tabs);
			writeStringVar(f, "texture-file", getLocalFile(t->getTextureRef()->getFilename()).c_str(), tabs);

			tabs--;
			writeTabs(f, tabs);
			M_fprintf(f, ")\n");
		}

        tabs--;

		writeTabs(f, tabs);
		M_fprintf(f, ")\n");
	}

	writeIntegerVar(f, "material-count", e->getMesh()->getMaterialsNumber(), tabs);
	writeTabs(f, tabs);
	M_fprintf(f, "(materials\n");
	tabs++;

	for(int i = 0; i < e->getMaterialsNumber(); i++)
	{
		Material* m = e->getMaterial(i);

		writeTabs(f, tabs);
		M_fprintf(f, "(material\n");
		tabs++;

		writeIntegerVar(f, "blend-mode", m->getBlendMode(), tabs);
		writeFloatVar(f, "opacity", m->getOpacity(), tabs);
		writeFloatVar(f, "shininess", m->getShininess(), tabs);
		writeFloatVar(f, "custom-value", m->getCustomValue(), tabs);
		writeVector3(f, "custom-color", m->getCustomColor(), tabs);
		writeVector3(f, "diffuse-color", m->getDiffuse(), tabs);
		writeVector3(f, "specular-color", m->getSpecular(), tabs);
		writeVector3(f, "emit-color", m->getEmit(), tabs);

		// Texture passes
		{
			writeIntegerVar(f, "pass-count", m->getTexturesPassNumber(), tabs);

			writeTabs(f, tabs);
			M_fprintf(f, "(texture-passes\n");
			tabs++;

			for(int j = 0; j < m->getTexturesPassNumber(); j++)
			{
				TexturePass* t = m->getTexturePass(j);

				writeTabs(f, tabs);
				M_fprintf(f, "(texture-pass\n");
				tabs++;

				writeIntegerVar(f, "combine-mode", t->getCombineMode(), tabs);
				writeIntegerVar(f, "map-channel", t->getMapChannel(), tabs);
				writeIntegerVar(f, "texture-id", findTextureId(t->getTexture(), e->getMesh()), tabs);

				tabs--;
				writeTabs(f, tabs);
				M_fprintf(f, ")\n");
			}

			tabs--;
			writeTabs(f, tabs);
			M_fprintf(f, ")\n");
		}

		tabs--;
		writeTabs(f, tabs);
		M_fprintf(f, ")\n");
	}

	tabs--;
	writeTabs(f, tabs);
	M_fprintf(f, ")\n");
}

void writeObject(File* f, Object3d* o, int tabs)
{
	writeTabs(f, tabs);
	M_fprintf(f, "(object\n");

	tabs++;

	writeStringVar(f, "name", o->getName(), tabs);
	if(o->hasParent())
		writeStringVar(f, "parent", o->getParent()->getName(), tabs);

	writeTransform(f, o, tabs);

	switch(o->getType())
	{
		case OBJECT3D_ENTITY:
			writeEntity(f, static_cast<OEntity*>(o), tabs);
			break;

		case OBJECT3D_LIGHT:
			writeLight(f, static_cast<OLight*>(o), tabs);
			break;

		case OBJECT3D_TEXT:
			writeText(f, static_cast<OText*>(o), tabs);
			break;

		case OBJECT3D_CAMERA:
			writeCamera(f, static_cast<OCamera*>(o), tabs);
			break;

		case OBJECT3D_SOUND:
			writeSound(f, static_cast<OSound*>(o), tabs);
			break;

		case OBJECT3D:
			writeStringVar(f, "type", "Group", tabs);
			break;
	}

	tabs--;

	writeTabs(f, tabs);
	M_fprintf(f, ")\n");
}

bool saveLispLevel(const char* filename, const char* type, Level* level)
{
	if(strcmp(type, "llvl"))
	{
		return false;
	}

	bool success = true;
	File* file = M_fopen(filename, "wt");
	if(!file)
		return false;

	int tabs = 0;
	char localFile[256];
	getRepertory(g_rep, filename);

	M_fprintf(file, "(level \n");
	tabs++;

	writeTabs(file, tabs);
	writeStringVar(file, "version", LEVEL_VERSION_STRING);

	writeTabs(file, tabs);
	writeIntegerVar(file, "currentScene", level->getCurrentSceneId());

	writeTabs(file, tabs);
	M_fprintf(file, "(scenes \n");
	tabs++;
	for(int i = 0; i < level->getScenesNumber(); i++)
	{
		Scene* s = level->getSceneByIndex(i);

		writeTabs(file, tabs);
		M_fprintf(file, "(scene \n");

		tabs++;
		writeTabs(file, tabs);
		writeStringVar(file, "name", s->getName());

		if(s->getScriptFilename())
		{
			if(strlen(s->getScriptFilename()) > 0)
			{
				getLocalFilename(localFile, g_rep, s->getScriptFilename());
				writeTabs(file, tabs);
				writeStringVar(file, "script", localFile);
			}
		}

		writeTabs(file, tabs);
		M_fprintf(file, "(objects\n");
		tabs++;
		for(int j = 0; j < s->getObjectsNumber(); j++)
		{
			writeObject(file, s->getObjectByIndex(j), tabs);
		}
		tabs--;
		writeTabs(file, tabs);
		M_fprintf(file, ")\n");

		tabs--;
		writeTabs(file, tabs);
		M_fprintf(file, ")\n");
	}
	tabs--;

	writeTabs(file, tabs);
	M_fprintf(file, ")\n)");
	M_fclose(file);
	return success;
}
