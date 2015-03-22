//========================================================================
// Copyright (c) 2003-2012 Anael Seghezzi <www.maratis3d.com>
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
#include "BinMeshLoader.h"

namespace Neo
{

// tools
static void readKey(File * file, Key * key, M_VARIABLE_TYPE type)
{
	int t;
	M_fread(&t, sizeof(int), 1, file);
	key->setT(t);

	switch(type)
	{
		case M_VARIABLE_FLOAT:
		{
			float * data = key->createFloatData();
			M_fread(data, sizeof(float), 1, file);
			break;
		}
		case M_VARIABLE_VEC2:
		{
			Vector2 * data = key->createVector2Data();
			M_fread(data, sizeof(Vector2), 1, file);
			break;
		}
		case M_VARIABLE_VEC3:
		{
			Vector3 * data = key->createVector3Data();
			M_fread(data, sizeof(Vector3), 1, file);
			break;
		}
		case M_VARIABLE_VEC4:
		{
			Vector4 * data = key->createVector4Data();
			M_fread(data, sizeof(Vector4), 1, file);
			break;
		}
		case M_VARIABLE_QUAT:
		{
			Quaternion * data = key->createQuaternionData();
			M_fread(data, sizeof(Quaternion), 1, file);
			break;
		}
	}
}

static unsigned int readKeysNumber(File * file)
{
	unsigned int keysNumber;
	M_fread(&keysNumber, sizeof(int), 1, file);
	return keysNumber;
}

static void readKeys(File * file, Key * keys, M_VARIABLE_TYPE type, unsigned int keysNumber)
{
	unsigned int k;
	for(k=0; k<keysNumber; k++)
		readKey(file, &(keys[k]), type);
}

static void readString(File * file, char * str)
{
	unsigned int l;
	M_fread(&l, sizeof(int), 1, file);
	if(l > 1)
		M_fread(str, sizeof(char), l, file);
}

static bool readDataRef(File * file, char * filename, const char * rep)
{
	char localFile[256];
	bool state;

	M_fread(&state, sizeof(bool), 1, file);
	if(state)
	{
		readString(file, localFile);
		getGlobalFilename(filename, rep, localFile);
	}

	return state;
}


// Mesh bin loading
bool M_loadBinMesh(const char * filename, void * data)
{
	char rep[256], path[256];
	bool state;

	// open file
	File * file = M_fopen(filename, "rb");
	if(! file)
	{
		MLOG_WARNING("Can't read file " << (filename?filename:"NULL"));
		return false;
	}

	// get engine, level, mesh
	NeoEngine * engine = NeoEngine().getInstance();
	Level * level = engine->getLevel();
	Mesh * mesh = (Mesh *)data;
	
	mesh->clear();
	

	// get file directory
	getRepertory(rep, filename);


	// header
	char header[8];
	M_fread(header, sizeof(char), 8, file);
	if(strcmp(header, "MMESH") != 0)
		return false;

	// version
	int version;
	M_fread(&version, sizeof(int), 1, file);


	// Animation
	{
		// armature anim ref
		if(readDataRef(file, path, rep))
		{
			ArmatureAnimRef * armatureAnim = level->loadArmatureAnim(path);
			mesh->setArmatureAnimRef(armatureAnim);
		}

		// textures anim ref
		if(readDataRef(file, path, rep))
		{
			TexturesAnimRef * texturesAnim = level->loadTexturesAnim(path);
			mesh->setTexturesAnimRef(texturesAnim);
		}

		// materials anim ref
		if(readDataRef(file, path, rep))
		{
			MaterialsAnimRef * materialsAnim = level->loadMaterialsAnim(path);
			mesh->setMaterialsAnimRef(materialsAnim);
		}

		// anims ranges
		unsigned int animsRangesNumber;
		AnimRange * animsRanges;

		M_fread(&animsRangesNumber, sizeof(int), 1, file);
		if(animsRangesNumber > 0)
		{
			animsRanges = mesh->allocAnimsRanges(animsRangesNumber);
			M_fread(animsRanges, sizeof(AnimRange), animsRangesNumber, file);
		}
	}


	// Textures
	{
		unsigned int t, texturesNumber;
		M_fread(&texturesNumber, sizeof(int), 1, file);

		if(texturesNumber > 0)
		{
			mesh->allocTextures(texturesNumber);
			for(t=0; t<texturesNumber; t++)
			{
				Texture * texture = mesh->addNewTexture(NULL);

				TEX_GEN_MODES genMode;
				WRAP_MODES UWrapMode;
				WRAP_MODES VWrapMode;
				Vector2 texTranslate;
				Vector2 texScale;
				float texRotate;

				// texture ref
				if(readDataRef(file, path, rep))
				{
					bool mipmap = true;
					if(version >= 2)
						M_fread(&mipmap, sizeof(bool), 1, file);
					
					TextureRef * textureRef = level->loadTexture(path, mipmap);
					texture->setTextureRef(textureRef);
				}

				// data
				M_fread(&genMode, sizeof(TEX_GEN_MODES), 1, file);
				M_fread(&UWrapMode, sizeof(WRAP_MODES), 1, file);
				M_fread(&VWrapMode, sizeof(WRAP_MODES), 1, file);
				M_fread(&texTranslate, sizeof(Vector2), 1, file);
				M_fread(&texScale, sizeof(Vector2), 1, file);
				M_fread(&texRotate, sizeof(float), 1, file);

				texture->setGenMode(genMode);
				texture->setUWrapMode(UWrapMode);
				texture->setVWrapMode(VWrapMode);
				texture->setTexRotate(texRotate);
				texture->setTexTranslate(texTranslate);
				texture->setTexScale(texScale);
			}
		}
	}


	// Materials
	{
		unsigned int m, materialsNumber;
		M_fread(&materialsNumber, sizeof(int), 1, file);
		if(materialsNumber > 0)
		{
			mesh->allocMaterials(materialsNumber);

			for(m=0; m<materialsNumber; m++)
			{
				Material * material = 	mesh->addNewMaterial();

				int type;
				float opacity;
				float shininess;
				float customValue;
				BLENDING_MODES blendMode;
				Vector3 emit, diffuse, specular, customColor;

				
				// FX ref
				M_fread(&state, sizeof(bool), 1, file);
				if(state)
				{
					ShaderRef * vertShadRef = NULL;
					ShaderRef * pixShadRef = NULL;

					if(readDataRef(file, path, rep))
						vertShadRef = level->loadShader(path, M_SHADER_VERTEX);
					if(readDataRef(file, path, rep))
						pixShadRef = level->loadShader(path, M_SHADER_PIXEL);

					if(vertShadRef && pixShadRef)
					{
						FXRef * FXRef = level->createFX(vertShadRef, pixShadRef);
						material->setFXRef(FXRef);
					}
				}

				// Z FX ref
				M_fread(&state, sizeof(bool), 1, file);
				if(state)
				{
					ShaderRef * vertShadRef = NULL;
					ShaderRef * pixShadRef = NULL;

					if(readDataRef(file, path, rep))
						vertShadRef = level->loadShader(path, M_SHADER_VERTEX);
					if(readDataRef(file, path, rep))
						pixShadRef = level->loadShader(path, M_SHADER_PIXEL);

					if(vertShadRef && pixShadRef)
					{
						FXRef * ZFXRef = level->createFX(vertShadRef, pixShadRef);
						material->setZFXRef(ZFXRef);
					}
				}

				// data
				M_fread(&type, sizeof(int), 1, file);
				M_fread(&opacity, sizeof(float), 1, file);
				M_fread(&shininess, sizeof(float), 1, file);
				M_fread(&customValue, sizeof(float), 1, file);
				M_fread(&blendMode, sizeof(BLENDING_MODES), 1, file);
				M_fread(&emit, sizeof(Vector3), 1, file);
				M_fread(&diffuse, sizeof(Vector3), 1, file);
				M_fread(&specular, sizeof(Vector3), 1, file);
				M_fread(&customColor, sizeof(Vector3), 1, file);

				material->setType(type);
				material->setOpacity(opacity);
				material->setShininess(shininess);
				material->setCustomValue(customValue);
				material->setBlendMode(blendMode);
				material->setEmit(emit);
				material->setDiffuse(diffuse);
				material->setSpecular(specular);
				material->setCustomColor(customColor);

				// textures pass
				unsigned int t, texturesPassNumber;
				M_fread(&texturesPassNumber, sizeof(int), 1, file);
				if(texturesPassNumber > 0)
				{
					material->allocTexturesPass(texturesPassNumber);

					for(t=0; t<texturesPassNumber; t++)
					{
						unsigned int mapChannel;
						TEX_COMBINE_MODES combineMode;

						// texture id
						int textureId;
						M_fread(&textureId, sizeof(int), 1, file);

						// data
						M_fread(&mapChannel, sizeof(int), 1, file);
						M_fread(&combineMode, sizeof(TEX_COMBINE_MODES), 1, file);

						Texture * texture = NULL;
						if(textureId >= 0)
							texture = mesh->getTexture(textureId);

						material->addTexturePass(texture, combineMode, mapChannel);
					}
				}
			}
		}
	}


	// Bones
	{
		M_fread(&state, sizeof(bool), 1, file);
		if(state)
		{
			Armature * armature = mesh->createArmature();

			unsigned int b, bonesNumber = armature->getBonesNumber();
			M_fread(&bonesNumber, sizeof(int), 1, file);
			if(bonesNumber > 0)
			{
				char name[256];

				// allocate bones
				armature->allocBones(bonesNumber);

				// add bones
				for(b=0; b<bonesNumber; b++)
					armature->addNewBone();

				// read bones
				for(b=0; b<bonesNumber; b++)
				{
					OBone * bone = armature->getBone(b);

					int parentId;
					Vector3 position;
					Vector3 scale;
					Quaternion rotation;

					// name
					readString(file, name);
					bone->setName(name);

					// parent id
					M_fread(&parentId, sizeof(int), 1, file);
					if(parentId >= 0)
						bone->linkTo(armature->getBone(parentId));

					// position / rotation / scale
					M_fread(&position, sizeof(Vector3), 1, file);
					M_fread(&rotation, sizeof(Quaternion), 1, file);
					M_fread(&scale, sizeof(Vector3), 1, file);

					bone->setPosition(position);
					bone->setRotation(rotation);
					bone->setScale(scale);
				}

				// construct bones inverse pose matrix
				armature->constructBonesInversePoseMatrix();
			}
		}
	}


	// BoundingBox
	{
		Box3d * box = mesh->getBoundingBox();
		M_fread(box, sizeof(Box3d), 1, file);
	}


	// SubMeshs
	{
		unsigned int s, subMeshsNumber;
		M_fread(&subMeshsNumber, sizeof(int), 1, file);

		if(subMeshsNumber > 0)
		{
			mesh->allocSubMeshs(subMeshsNumber);

			SubMesh * subMeshs = mesh->getSubMeshs();
			for(s=0; s<subMeshsNumber; s++)
			{
				SubMesh * subMesh = &(subMeshs[s]);

				unsigned int indicesSize;
				unsigned int verticesSize;
				unsigned int normalsSize;
				unsigned int tangentsSize;
				unsigned int texCoordsSize;
				unsigned int colorsSize;

				VAR_TYPES indicesType;
				void * indices;

				Color * colors;
				Vector3 * vertices;
				Vector3 * normals;
				Vector3 * tangents;
				Vector2 * texCoords;

				Box3d * box = subMesh->getBoundingBox();
				SkinData * skin;


				// BoundingBox
				M_fread(box, sizeof(Box3d), 1, file);

				// indices
				M_fread(&indicesSize, sizeof(int), 1, file);
				if(indicesSize > 0)
				{
					// indice type
					M_fread(&indicesType, sizeof(VAR_TYPES), 1, file);
					indices = subMesh->allocIndices(indicesSize, indicesType);

					switch(indicesType)
					{
						case VAR_USHORT:
							M_fread(indices, sizeof(short), indicesSize, file);
							break;
						case VAR_UINT:
							M_fread(indices, sizeof(int), indicesSize, file);
							break;
					}
				}

				// vertices
				M_fread(&verticesSize, sizeof(int), 1, file);
				if(verticesSize > 0)
				{
					vertices = subMesh->allocVertices(verticesSize);
					M_fread(vertices, sizeof(Vector3), verticesSize, file);
				}

				// normals
				M_fread(&normalsSize, sizeof(int), 1, file);
				if(normalsSize > 0)
				{
					normals = subMesh->allocNormals(normalsSize);
					M_fread(normals, sizeof(Vector3), normalsSize, file);
				}

				// tangents
				M_fread(&tangentsSize, sizeof(int), 1, file);
				if(tangentsSize > 0)
				{
					tangents = subMesh->allocTangents(tangentsSize);
					M_fread(tangents, sizeof(Vector3), tangentsSize, file);
				}

				// texCoords
				M_fread(&texCoordsSize, sizeof(int), 1, file);
				if(texCoordsSize > 0)
				{
					texCoords = subMesh->allocTexCoords(texCoordsSize);
					M_fread(texCoords, sizeof(Vector2), texCoordsSize, file);
				}

				// colors
				M_fread(&colorsSize, sizeof(int), 1, file);
				if(colorsSize > 0)
				{
					colors = subMesh->allocColors(colorsSize);
					M_fread(colors, sizeof(Color), colorsSize, file);
				}


				// mapChannels
				{
					unsigned int i, size, channel, offset;
					M_fread(&size, sizeof(int), 1, file);

					for(i=0; i<size; i++)
					{
						M_fread(&channel, sizeof(int), 1, file);
						M_fread(&offset, sizeof(int), 1, file);
						subMesh->setMapChannelOffset(channel, offset);
					}
				}


				// Skins
				M_fread(&state, sizeof(bool), 1, file);
				if(state)
				{
					skin = subMesh->createSkinData();

					// skin point
					unsigned int p, pointsNumber;
					M_fread(&pointsNumber, sizeof(int), 1, file);
					if(pointsNumber > 0)
					{
						skin->allocPoints(pointsNumber);
						for(p=0; p<pointsNumber; p++)
						{
							SkinPoint * skinPoint = skin->getPoint(p);

							unsigned int vertexId;
							unsigned int bonesNumber;
							unsigned short * bonesIds;
							float * bonesWeights;

							// vertex id
							M_fread(&vertexId, sizeof(int), 1, file);
							skinPoint->setVertexId(vertexId);

							// bones
							M_fread(&bonesNumber, sizeof(int), 1, file);
							if(bonesNumber > 0)
							{
								skinPoint->allocateBonesLinks(bonesNumber);
								bonesIds = skinPoint->getBonesIds();
								bonesWeights = skinPoint->getBonesWeights();

								M_fread(bonesIds, sizeof(short), bonesNumber, file);
								M_fread(bonesWeights, sizeof(float), bonesNumber, file);
							}
						}
					}
				}


				// Displays
				unsigned int d, displaysNumber;
				M_fread(&displaysNumber, sizeof(int), 1, file);
				if(displaysNumber > 0)
				{
					subMesh->allocDisplays(displaysNumber);

					for(d=0; d<displaysNumber; d++)
					{
						unsigned int begin;
						unsigned int size;
						CULL_MODES cullMode;
						PRIMITIVE_TYPES primitiveType;

						int materialId;

						// data
						M_fread(&primitiveType, sizeof(PRIMITIVE_TYPES), 1, file);
						M_fread(&begin, sizeof(int), 1, file);
						M_fread(&size, sizeof(int), 1, file);
						M_fread(&materialId, sizeof(int), 1, file);
						M_fread(&cullMode, sizeof(CULL_MODES), 1, file);

						// display
						MaterialDisplay * display = subMesh->addNewDisplay(primitiveType, begin, size);
						display->setCullMode(cullMode);

						if(materialId >= 0)
							display->setMaterial(mesh->getMaterial(materialId));
					}
				}
			}
		}
	}


	M_fclose(file);
	return true;
}


// Armature anim bin loading
bool M_loadBinArmatureAnim(const char * filename, void * data)
{
	int version;
	char header[8];


	// open file
	File * file = M_fopen(filename, "rb");
	if(! file)
	{
		fprintf(stderr, "Error : can't read file %s\n", filename);
		return false;
	}

	// data
	ArmatureAnim * anim = (ArmatureAnim *)data;


	// header
	M_fread(header, sizeof(char), 8, file);
	if(strcmp(header, "MAA") != 0)
		return false;

	// version
	M_fread(&version, sizeof(int), 1, file);


	// bones
	unsigned int b, bonesAnimNumber;
	M_fread(&bonesAnimNumber, sizeof(int), 1, file);
	{
		Object3dAnim * bonesAnim = anim->allocBonesAnim(bonesAnimNumber);

		for(b=0; b<bonesAnimNumber; b++)
		{
			Object3dAnim * objAnim = &(bonesAnim[b]);
			unsigned int keysNumber;
			Key * keys;

			// position keys
			keysNumber = readKeysNumber(file);
			keys = objAnim->allocPositionKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC3, keysNumber);

			// scale keys
			keysNumber = readKeysNumber(file);
			keys = objAnim->allocScaleKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC3, keysNumber);

			// rotation keys
			keysNumber = readKeysNumber(file);
			keys = objAnim->allocRotationKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_QUAT, keysNumber);
		}
	}


	M_fclose(file);
	return true;
}


// Textures anim bin loading
bool M_loadBinTexturesAnim(const char * filename, void * data)
{
	int version;
	char header[8];


	// open file
	File * file = M_fopen(filename, "rb");
	if(! file)
	{
		fprintf(stderr, "Error : can't read file %s\n", filename);
		return false;
	}

	// data
	TexturesAnim * anim = (TexturesAnim *)data;


	// header
	M_fread(header, sizeof(char), 8, file);
	if(strcmp(header, "MTA") != 0)
		return false;

	// version
	M_fread(&version, sizeof(int), 1, file);


	// textures
	unsigned int t, texturesAnimNumber;
	M_fread(&texturesAnimNumber, sizeof(int), 1, file);
	{
		TextureAnim * texturesAnim = anim->allocTexturesAnim(texturesAnimNumber);

		for(t=0; t<texturesAnimNumber; t++)
		{
			TextureAnim * texAnim = &(texturesAnim[t]);
			unsigned int keysNumber;
			Key * keys;

			// translate keys
			keysNumber = readKeysNumber(file);
			keys = texAnim->allocTranslateKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC2, keysNumber);

			// scale keys
			keysNumber = readKeysNumber(file);
			keys = texAnim->allocScaleKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC2, keysNumber);

			// rotation keys
			keysNumber = readKeysNumber(file);
			keys = texAnim->allocRotationKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_FLOAT, keysNumber);
		}
	}


	M_fclose(file);
	return true;
}


// Materials anim bin export
bool M_loadBinMaterialsAnim(const char * filename, void * data)
{
	int version;
	char header[8];


	// open file
	File * file = M_fopen(filename, "rb");
	if(! file)
	{
		fprintf(stderr, "Error : can't read file %s\n", filename);
		return false;
	}

	// data
	MaterialsAnim * anim = (MaterialsAnim *)data;


	// header
	M_fread(header, sizeof(char), 8, file);
	if(strcmp(header, "MMA") != 0)
		return false;

	// version
	M_fread(&version, sizeof(int), 1, file);


	// materials
	unsigned int m, materialsAnimNumber;
	M_fread(&materialsAnimNumber, sizeof(int), 1, file);
	{
		MaterialAnim * materialsAnim = anim->allocMaterialsAnim(materialsAnimNumber);

		for(m=0; m<materialsAnimNumber; m++)
		{
			MaterialAnim * matAnim = &(materialsAnim[m]);
			unsigned int keysNumber;
			Key * keys;

			// opacity keys
			keysNumber = readKeysNumber(file);
			keys = matAnim->allocOpacityKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_FLOAT, keysNumber);

			// shininess keys
			keysNumber = readKeysNumber(file);
			keys = matAnim->allocShininessKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_FLOAT, keysNumber);

			// custom value keys
			keysNumber = readKeysNumber(file);
			keys = matAnim->allocCustomValueKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_FLOAT, keysNumber);

			// diffuse keys
			keysNumber = readKeysNumber(file);
			keys = matAnim->allocDiffuseKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC3, keysNumber);

			// specular keys
			keysNumber = readKeysNumber(file);
			keys = matAnim->allocSpecularKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC3, keysNumber);

			// emit keys
			keysNumber = readKeysNumber(file);
			keys = matAnim->allocEmitKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC3, keysNumber);

			// custom color keys
			keysNumber = readKeysNumber(file);
			keys = matAnim->allocCustomColorKeys(keysNumber);
			readKeys(file, keys, M_VARIABLE_VEC3, keysNumber);
		}
	}


	M_fclose(file);
	return true;
}
}
