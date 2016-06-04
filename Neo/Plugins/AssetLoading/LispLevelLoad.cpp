//========================================================================
// Copyright (c) 2016 Yannick Pflanzer <www.neo-engine.de>
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

#define OPT_OUT_PIKE_STYLE
#include <sexpresso.hpp>

using namespace Neo;

static char rep[256];
static Level* level;

struct Link
{
	Link(Object3d* o, const string& parent) :
		object(o),
		parent(std::move(parent))
	{}

	Object3d* object;
	string parent;
};

std::string GET_VALUE(sexpresso::Sexp& node, const char* name)
{
	auto n = node.getChildByPath(name);
	if(!n)
		return "";

	return n->arguments().begin()->value.str;
}

Vector3 GET_VECTOR3_VALUE(sexpresso::Sexp& node, const char* name)
{
	auto cn = node.getChildByPath(name);
	if(!cn)
		return Vector3();

	if(cn->arguments().size() < 3)
		return Vector3();

	sexpresso::Sexp& xval = *(cn->arguments().begin());
	sexpresso::Sexp& yval = *(cn->arguments().begin() + 1);
	sexpresso::Sexp& zval = *(cn->arguments().begin() + 2);

	return Vector3(stof(xval.value.str),
				   stof(yval.value.str),
				   stof(zval.value.str));
}

Vector2 GET_VECTOR2_VALUE(sexpresso::Sexp& node, const char* name)
{
	auto cn = node.getChildByPath(name);
	if(!cn)
		return Vector2();

	if(cn->arguments().size() < 2)
		return Vector2();

	sexpresso::Sexp& xval = *(cn->arguments().begin());
	sexpresso::Sexp& yval = *(cn->arguments().begin() + 1);

	return Vector2(stof(xval.value.str),
				   stof(yval.value.str));
}

float GET_FLOAT_VALUE(sexpresso::Sexp& node, const char* name)
{
	auto cn = node.getChildByPath(name);

	if(!cn)
		return 0.0f;

	return stof(cn->arguments().begin()->value.str);
}

int GET_INT_VALUE(sexpresso::Sexp& node, const char* name)
{
	auto cn = node.getChildByPath(name);

	if(!cn)
		return 0;

	return stod(cn->arguments().begin()->value.str);
}

Vector3 GET_VECTOR4_VALUE(sexpresso::Sexp& node, const char* name)
{
	auto cn = node.getChildByPath(name);
	if(!cn)
		return Vector3();

	if(cn->arguments().size() < 4)
		return Vector3();

	sexpresso::Sexp& xval = *(cn->arguments().begin());
	sexpresso::Sexp& yval = *(cn->arguments().begin() + 1);
	sexpresso::Sexp& zval = *(cn->arguments().begin() + 2);
	sexpresso::Sexp& wval = *(cn->arguments().begin() + 3);

	return Vector4(stof(xval.value.str),
				   stof(yval.value.str),
				   stof(zval.value.str),
				   stof(wval.value.str));
}

bool loadLispLevel(const char* filename, Level* level)
{
	char* levelfile = readTextFile(filename);
	if(!levelfile)
	{
		MLOG_ERROR("Could not load level file: " << filename);
		return false;
	}

	string error;
	auto root = sexpresso::parse(levelfile, error);
	SAFE_DELETE(levelfile);

	if(!error.empty())
	{
		MLOG_ERROR("Could not load level: " << error);
		return false;
	}

	auto levelNode = root.getChildByPath("level");
	if(!levelNode)
	{
		MLOG_ERROR("Could not find level node!");
		return false;
	}

	std::vector<Link> parentChildLinks;

	// Load scenes
	{
		auto sceneNode = levelNode->getChildByPath("scenes");
		if(!sceneNode)
		{
			MLOG_ERROR("Could not find scene list!");
			return false;
		}

		for(auto sn : sceneNode->arguments())
		{
			// Load scene
			Scene* s = level->addNewScene();
			s->setName(GET_VALUE(sn, "name").c_str());

			if(auto v = sn.getChildByPath("script"))
				s->setScriptFilename(v->arguments().begin()->value.str.c_str());

			auto on = sn.getChildByPath("objects");
			if(!on)
				return false;

			for(auto o : on->arguments())
			{
				Object3d* object = nullptr;
				auto type = GET_VALUE(o, "type");
				if(type == "Light")
				{
					auto light = s->addNewLight();
					object = light;

					light->setRadius(stof(GET_VALUE(o, "radius")));
					light->setColor(GET_VECTOR3_VALUE(o, "color"));
					light->setIntensity(GET_FLOAT_VALUE(o, "intensity"));
					light->setSpotAngle(GET_FLOAT_VALUE(o, "spot-angle"));
					light->setSpotExponent(GET_FLOAT_VALUE(o, "spot-exponent"));
					light->setShadowBias(GET_FLOAT_VALUE(o, "shadow-bias"));
					light->setShadowBlur(GET_FLOAT_VALUE(o, "shadow-blur"));
					light->setShadowQuality(GET_INT_VALUE(o, "shadow-quality"));
					light->castShadow(GET_INT_VALUE(o, "has-shadow"));
				}
				else if(type == "Entity")
				{
					OEntity* entity = s->addNewEntity(level->loadMesh(GET_VALUE(o, "mesh").c_str()));
					object = entity;

					Mesh* mesh = entity->getMesh();
					entity->enableShadow(GET_INT_VALUE(o, "has-shadow"));

					std::vector<Texture*> textureList;
					// Load textures
					auto tn = o.getChildByPath("textures");
					if(tn)
					{
						entity->getMesh()->allocTextures(GET_INT_VALUE(o, "texture-count"));
						for(auto t : tn->arguments())
						{
							TextureRef* texref = level->loadTexture(GET_VALUE(t, "texture-file").c_str());
							Texture* tex = new Texture(texref);

							tex->setGenMode(static_cast<TEX_GEN_MODES>(GET_INT_VALUE(t, "gen-mode")));
							tex->setUWrapMode(static_cast<WRAP_MODES>(GET_INT_VALUE(t, "uw-wrap")));
							tex->setVWrapMode(static_cast<WRAP_MODES>(GET_INT_VALUE(t, "vw-wrap")));
							tex->setTexTranslate(GET_VECTOR2_VALUE(t, "tex-translate"));
							tex->setTexRotate(GET_FLOAT_VALUE(t, "tex-rotate"));
							tex->setTexScale(GET_VECTOR2_VALUE(t, "tex-scale"));

							textureList.push_back(tex);
						}
					}

					// Load Materials
					tn = o.getChildByPath("materials");
					if(tn)
					{
						int matId = 0;
						for(auto t : tn->arguments())
						{
							if(matId >= entity->getMaterialsNumber())
								break;

							Material* mat = entity->getMaterial(matId++);
							mat->setBlendMode(static_cast<BLENDING_MODES>(GET_INT_VALUE(t, "blend-mode")));
							mat->setOpacity(GET_FLOAT_VALUE(t, "opacity"));
							mat->setShininess(GET_FLOAT_VALUE(t, "shininess"));
							mat->setCustomValue(GET_FLOAT_VALUE(t, "custom-value"));
							mat->setCustomColor(GET_VECTOR3_VALUE(t, "custom-color"));
							mat->setDiffuse(GET_VECTOR3_VALUE(t, "diffuse-color"));
							mat->setSpecular(GET_VECTOR3_VALUE(t, "specular-color"));
							mat->setEmit(GET_VECTOR3_VALUE(t, "emit-color"));

							// Texture passes
							{
								// Reset the texture pass counter.
								// This call does not produce a memory leak since the
								// passes used were copied from the mesh where they still
								// reside.
								mat->resetTexturesPassNumber();
								auto p = t.getChildByPath("texture-passes");
								if(p)
								{
									for(auto pass : p->arguments())
									{
										mat->addTexturePass(textureList[GET_INT_VALUE(pass, "texture-id")],
															static_cast<TEX_COMBINE_MODES>(GET_INT_VALUE(pass,
																										 "combine-mode")),
															GET_INT_VALUE(pass, "map-channel"));

									}
								}
							}
						}
					}
				}
				else if(type == "Camera")
				{
					auto camera = s->addNewCamera();
					object = camera;

					camera->setFogDistance(GET_FLOAT_VALUE(o, "fog-distance"));
					camera->setFogColor(GET_VECTOR3_VALUE(o, "fog-color"));
					camera->setClearColor(GET_VECTOR3_VALUE(o, "clear-color"));
					camera->setFov(GET_FLOAT_VALUE(o, "fov"));
					camera->setClippingNear(GET_FLOAT_VALUE(o, "clipping-near"));
					camera->setClippingFar(GET_FLOAT_VALUE(o, "clipping-far"));
					camera->enableOrtho(GET_INT_VALUE(o, "is-ortho"));
					camera->enableFog(GET_INT_VALUE(o, "has-fog"));
				}
				else if(type == "Text")
				{
					auto text = s->addNewText(level->loadFont(GET_VALUE(o, "font").c_str()));
					object = text;

					text->setText(GET_VALUE(o, "text").c_str());
					text->setColor(GET_VECTOR4_VALUE(o, "color"));
					text->setAlign(static_cast<TEXT_ALIGN_MODES>(GET_INT_VALUE(o, "alignment")));
					text->setSize(GET_FLOAT_VALUE(o, "size"));
				}
				else if(type == "Sound")
				{
					auto sound = s->addNewSound(level->loadSound(GET_VALUE(o, "sound").c_str()));
					object = sound;

					sound->setPitch(GET_FLOAT_VALUE(o, "pitch"));
					sound->setGain(GET_FLOAT_VALUE(o, "gain"));
					sound->setRadius(GET_FLOAT_VALUE(o, "radius"));
					sound->setRolloff(GET_FLOAT_VALUE(o, "rolloff"));
					sound->setLooping(GET_INT_VALUE(o, "is-looping"));
					sound->setRelative(GET_INT_VALUE(o, "is-relative"));
				}
				else if(type == "Group")
				{
					object = s->addNewGroup();
				}

				if(!object) continue;
				object->setName(GET_VALUE(o, "name").c_str());

				auto tn = o.getChildByPath("transform");
				if(!tn)
					return false;

				object->setPosition(GET_VECTOR3_VALUE(*tn, "position"));
				object->setEulerRotation(GET_VECTOR3_VALUE(*tn, "rotation"));
				object->setScale(GET_VECTOR3_VALUE(*tn, "scale"));

				std::string parent = GET_VALUE(o, "parent");
				if(!parent.empty())
				{
					parentChildLinks.push_back(Link(object, parent));
				}
			}

			// Link parent-child relationships
			for(auto l : parentChildLinks)
			{
				l.object->linkTo(s->getObjectByName(l.parent.c_str()));
			}
		}
	}

	return true;
}
