//========================================================================
//  Copyright (c) 2003-2012 Anael Seghezzi <www.maratis3d.com>
//	Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================

#include <NeoEngine.h>
#include "MeshBin.h"

using namespace Neo;

static char M_MESH_HEADER[8] = {'M', 'M', 'E', 'S', 'H', '\0', '\0', '\0'}; // mesh
static char M_AA_HEADER[8] = {'M', 'A', 'A', '\0', '\0', '\0', '\0', '\0'}; // armature anim
static char M_MA_HEADER[8] = {'M', 'M', 'A', '\0', '\0', '\0', '\0', '\0'}; // materials anim
static char M_TA_HEADER[8] = {'M', 'T', 'A', '\0', '\0', '\0', '\0', '\0'}; // textures anim

// tools
static void writeString(File * file, const char * str)
{
	unsigned int l = strlen(str) + 1;
	
	M_fwrite(&l, sizeof(int), 1, file);
	if(l > 1)
		M_fwrite(str, sizeof(char), l, file);
}

static void writeKey(File * file, Key * key, M_VARIABLE_TYPE type)
{
	int t = key->getT();
	M_fwrite(&t, sizeof(int), 1, file);
	
	switch(type)
	{
		case M_VARIABLE_FLOAT:
		{
			float * data = (float *)key->getData();
			M_fwrite(data, sizeof(float), 1, file);
			break;
		}
		case M_VARIABLE_VEC2:
		{
			Vector2 * data = (Vector2 *)key->getData();
			M_fwrite(data, sizeof(Vector2), 1, file);
			break;
		}
		case M_VARIABLE_VEC3:
		{
			Vector3 * data = (Vector3 *)key->getData();
			M_fwrite(data, sizeof(Vector3), 1, file);
			break;
		}
		case M_VARIABLE_VEC4:
		{
			Vector4 * data = (Vector4 *)key->getData();
			M_fwrite(data, sizeof(Vector4), 1, file);
			break;
		}
		case M_VARIABLE_QUAT:
		{
			Quaternion * data = (Quaternion *)key->getData();
			M_fwrite(data, sizeof(Quaternion), 1, file);
			break;
		}
	}
}

static void writeKeys(File * file, Key * keys, M_VARIABLE_TYPE type, unsigned int keysNumber)
{
	unsigned int k;
	
	M_fwrite(&keysNumber, sizeof(int), 1, file);
	for(k=0; k<keysNumber; k++)
		writeKey(file, &(keys[k]), type);
}

static void writeDataRef(File * file, DataRef * dataRef, const char * rep)
{
	char localFile[256];
	
	bool state = false;
	if(dataRef)
	{
		const char * filename = dataRef->getFilename();
		if(filename != NULL)
			state = true;
	}
	
	M_fwrite(&state, sizeof(bool), 1, file);
	if(state)
	{
		getLocalFilename(localFile, rep, dataRef->getFilename());
		writeString(file, localFile);
	}
}

static int getTextureId(Mesh * mesh, Texture * texture)
{
	unsigned int t, texturesNumber = mesh->getTexturesNumber();
	for(t=0; t<texturesNumber; t++)
	{
		if(mesh->getTexture(t) == texture)
			return t;
	}
	
	return -1;
}

static int getMaterialId(Mesh * mesh, Material * material)
{
	unsigned int m, materialsNumber = mesh->getMaterialsNumber();
	for(m=0; m<materialsNumber; m++)
	{
		if(mesh->getMaterial(m) == material)
			return m;
	}
	
	return -1;
}


// Mesh bin export
bool exportMeshBin(const char * filename, Mesh * mesh)
{
	int version = 2;
	char rep[256];
	bool state;
	
	
	// create file
	File * file = M_fopen(filename, "wb");
	if(! file)
	{
		fprintf(stderr, "Error : can't create file %s\n", filename);
		return false;
	}
	
	
	// get file directory
	getRepertory(rep, filename);
	
	
	// header
	M_fwrite(M_MESH_HEADER, sizeof(char), 8, file);

	// version
	M_fwrite(&version, sizeof(int), 1, file);


	// Animation
	{
		// anim refs
		ArmatureAnimRef * armatureAnimRef = mesh->getArmatureAnimRef();
		TexturesAnimRef * texturesAnimRef = mesh->getTexturesAnimRef();
		MaterialsAnimRef * materialsAnimRef = mesh->getMaterialsAnimRef();
		
		// armature anim ref
		writeDataRef(file, armatureAnimRef, rep);
		
		// textures anim ref
		writeDataRef(file, texturesAnimRef, rep);
		
		// materials anim ref
		writeDataRef(file, materialsAnimRef, rep);
		
		// anims ranges
		unsigned int animsRangesNumber = mesh->getAnimsRangesNumber();
		AnimRange * animsRanges = mesh->getAnimsRanges();
		
		M_fwrite(&animsRangesNumber, sizeof(int), 1, file);
		if(animsRangesNumber > 0)
			M_fwrite(animsRanges, sizeof(AnimRange), animsRangesNumber, file);
	}
	
	
	// Textures
	{
		unsigned int t, texturesNumber = mesh->getTexturesNumber();
		M_fwrite(&texturesNumber, sizeof(int), 1, file);
		for(t=0; t<texturesNumber; t++)
		{
			Texture * texture = mesh->getTexture(t);
			
			TextureRef * textureRef = texture->getTextureRef();
			TEX_GEN_MODES genMode = texture->getGenMode();
			WRAP_MODES UWrapMode = texture->getUWrapMode();
			WRAP_MODES VWrapMode = texture->getVWrapMode();
			Vector2 texTranslate = texture->getTexTranslate();
			Vector2 texScale = texture->getTexScale();
			float texRotate = texture->getTexRotate();
			
			// texture ref
			writeDataRef(file, textureRef, rep);
			if(textureRef)
			{
				bool mipmap = textureRef->isMipmapEnabled();
				M_fwrite(&mipmap, sizeof(bool), 1, file);
			}
			
			// data
			M_fwrite(&genMode, sizeof(TEX_GEN_MODES), 1, file);
			M_fwrite(&UWrapMode, sizeof(WRAP_MODES), 1, file);
			M_fwrite(&VWrapMode, sizeof(WRAP_MODES), 1, file);
			M_fwrite(&texTranslate, sizeof(Vector2), 1, file);
			M_fwrite(&texScale, sizeof(Vector2), 1, file);
			M_fwrite(&texRotate, sizeof(float), 1, file);
		}
	}
	
	
	// Materials
	{
		unsigned int m, materialsNumber = mesh->getMaterialsNumber();
		M_fwrite(&materialsNumber, sizeof(int), 1, file);
		for(m=0; m<materialsNumber; m++)
		{
			Material * material = mesh->getMaterial(m);
			
			int type = material->getType();
			float opacity = material->getOpacity();
			float shininess = material->getShininess();
			float customValue = material->getCustomValue();
			BLENDING_MODES blendMode = material->getBlendMode();
			Vector3 emit = material->getEmit();
			Vector3 diffuse = material->getDiffuse();
			Vector3 specular = material->getSpecular();
			Vector3 customColor = material->getCustomColor();
			FXRef * fx_ref = material->getFXRef();
			FXRef * zfx_ref = material->getZFXRef();
			
			// FX ref
			state = fx_ref != NULL;
			M_fwrite(&state, sizeof(bool), 1, file);
			if(fx_ref)
			{
				ShaderRef * vertShadRef = fx_ref->getVertexShaderRef();
				ShaderRef * pixShadRef = fx_ref->getPixelShaderRef();
				
				writeDataRef(file, vertShadRef, rep);
				writeDataRef(file, pixShadRef, rep);
			}
			
			// Z FX ref
			state = zfx_ref != NULL;
			M_fwrite(&state, sizeof(bool), 1, file);
			if(zfx_ref)
			{
				ShaderRef * vertShadRef = zfx_ref->getVertexShaderRef();
				ShaderRef * pixShadRef = zfx_ref->getPixelShaderRef();
				
				writeDataRef(file, vertShadRef, rep);
				writeDataRef(file, pixShadRef, rep);
			}
			
			// data
			M_fwrite(&type, sizeof(int), 1, file);
			M_fwrite(&opacity, sizeof(float), 1, file);
			M_fwrite(&shininess, sizeof(float), 1, file);
			M_fwrite(&customValue, sizeof(float), 1, file);
			M_fwrite(&blendMode, sizeof(BLENDING_MODES), 1, file);
			M_fwrite(&emit, sizeof(Vector3), 1, file);
			M_fwrite(&diffuse, sizeof(Vector3), 1, file);
			M_fwrite(&specular, sizeof(Vector3), 1, file);
			M_fwrite(&customColor, sizeof(Vector3), 1, file);
			
			// textures pass
			unsigned int t, texturesPassNumber = material->getTexturesPassNumber();
			
			M_fwrite(&texturesPassNumber, sizeof(int), 1, file);
			for(t=0; t<texturesPassNumber; t++)
			{
				TexturePass * texturePass = material->getTexturePass(t);
				
				Texture * texture = texturePass->getTexture();
				unsigned int mapChannel = texturePass->getMapChannel();
				TEX_COMBINE_MODES combineMode = texturePass->getCombineMode();
				
				// texture id
				int textureId = getTextureId(mesh, texture);
				M_fwrite(&textureId, sizeof(int), 1, file);
				
				// data
				M_fwrite(&mapChannel, sizeof(int), 1, file);
				M_fwrite(&combineMode, sizeof(TEX_COMBINE_MODES), 1, file);
			}
		}
	}
	
	
	// Bones
	{
		Armature * armature = mesh->getArmature();
		
		state = armature != NULL;
		M_fwrite(&state, sizeof(bool), 1, file);
		if(armature)
		{
			unsigned int b, bonesNumber = armature->getBonesNumber();
			M_fwrite(&bonesNumber, sizeof(int), 1, file);
			for(b=0; b<bonesNumber; b++)
			{
				OBone * bone = armature->getBone(b);
				Object3d * parent = bone->getParent();
				
				Vector3 position = bone->getPosition();
				Vector3 scale = bone->getScale();
				Quaternion rotation = bone->getRotation();
				
				// name
				writeString(file, bone->getName());
				
				// parent id
				int parentId = -1;
				if(parent)
				{
					unsigned int id;
					if(armature->getBoneId(parent->getName(), &id))
						parentId = (int)id;
				}
				
				M_fwrite(&parentId, sizeof(int), 1, file);
				
				// position / rotation / scale
				M_fwrite(&position, sizeof(Vector3), 1, file);
				M_fwrite(&rotation, sizeof(Quaternion), 1, file);
				M_fwrite(&scale, sizeof(Vector3), 1, file);
			}
		}
	}
	
	
	// BoundingBox
	{
		Box3d * box = mesh->getBoundingBox();
		M_fwrite(box, sizeof(Box3d), 1, file);
	}
	
	
	// SubMeshs
	{
		unsigned int s, subMeshsNumber = mesh->getSubMeshsNumber();
		SubMesh * subMeshs = mesh->getSubMeshs();
		
		M_fwrite(&subMeshsNumber, sizeof(int), 1, file);
		for(s=0; s<subMeshsNumber; s++)
		{
			SubMesh * subMesh = &(subMeshs[s]);

			unsigned int indicesSize = subMesh->getIndicesSize();
			unsigned int verticesSize = subMesh->getVerticesSize();
			unsigned int normalsSize = subMesh->getNormalsSize();
			unsigned int tangentsSize = subMesh->getTangentsSize();
			unsigned int texCoordsSize = subMesh->getTexCoordsSize();
			unsigned int colorsSize = subMesh->getColorsSize();
			
			VAR_TYPES indicesType = subMesh->getIndicesType();
			void * indices = subMesh->getIndices();
			
			Color * colors = subMesh->getColors();
			Vector3 * vertices = subMesh->getVertices();
			Vector3 * normals = subMesh->getNormals();
			Vector3 * tangents = subMesh->getTangents();
			Vector2 * texCoords = subMesh->getTexCoords();
			
			Box3d * box = subMesh->getBoundingBox();
			SkinData * skin = subMesh->getSkinData();
			map<unsigned int, unsigned int> * mapChannelOffsets = subMesh->getMapChannelOffsets();
			
			
			// BoundingBox
			M_fwrite(box, sizeof(Box3d), 1, file);
			
			// indices
			M_fwrite(&indicesSize, sizeof(int), 1, file);
			if(indicesSize > 0)
			{
				// indice type
				M_fwrite(&indicesType, sizeof(VAR_TYPES), 1, file);
				switch(indicesType)
				{
					case VAR_USHORT:
						M_fwrite(indices, sizeof(short), indicesSize, file);
						break;
					case VAR_UINT:
						M_fwrite(indices, sizeof(int), indicesSize, file);
						break;
				}
			}
			
			// vertices
			M_fwrite(&verticesSize, sizeof(int), 1, file);
			if(verticesSize > 0)
				M_fwrite(vertices, sizeof(Vector3), verticesSize, file);
			
			// normals
			M_fwrite(&normalsSize, sizeof(int), 1, file);
			if(normalsSize > 0)
				M_fwrite(normals, sizeof(Vector3), normalsSize, file);
			
			// tangents
			M_fwrite(&tangentsSize, sizeof(int), 1, file);
			if(tangentsSize > 0)
				M_fwrite(tangents, sizeof(Vector3), tangentsSize, file);
			
			// texCoords
			M_fwrite(&texCoordsSize, sizeof(int), 1, file);
			if(texCoordsSize > 0)
				M_fwrite(texCoords, sizeof(Vector2), texCoordsSize, file);
			
			// colors
			M_fwrite(&colorsSize, sizeof(int), 1, file);
			if(colorsSize > 0)
				M_fwrite(colors, sizeof(Color), colorsSize, file);
			
			// mapChannels
			{
				unsigned int size = mapChannelOffsets->size();
				M_fwrite(&size, sizeof(int), 1, file);
				
				map<unsigned int, unsigned int>::iterator
				mit (mapChannelOffsets->begin()),
				mend(mapChannelOffsets->end());
				
				for(;mit!=mend;++mit)
				{
					M_fwrite(&mit->first, sizeof(int), 1, file);
					M_fwrite(&mit->second, sizeof(int), 1, file);
				}
			}
			
			
			// Skins
			state = skin != NULL;
			M_fwrite(&state, sizeof(bool), 1, file);
			if(skin)
			{
				// skin point
				unsigned int p, pointsNumber = skin->getPointsNumber();
				M_fwrite(&pointsNumber, sizeof(int), 1, file);
				for(p=0; p<pointsNumber; p++)
				{
					SkinPoint * skinPoint = skin->getPoint(p);
					
					unsigned int vertexId = skinPoint->getVertexId();
					unsigned int bonesNumber = skinPoint->getBonesNumber();
					unsigned short * bonesIds = skinPoint->getBonesIds();
					float * bonesWeights = skinPoint->getBonesWeights();
					
					// data
					M_fwrite(&vertexId, sizeof(int), 1, file);
					M_fwrite(&bonesNumber, sizeof(int), 1, file);
					if(bonesNumber > 0)
					{
						M_fwrite(bonesIds, sizeof(short), bonesNumber, file);
						M_fwrite(bonesWeights, sizeof(float), bonesNumber, file);
					}
				}
			}
			
			
			// Displays
			unsigned int d, displaysNumber = subMesh->getDisplaysNumber();
			M_fwrite(&displaysNumber, sizeof(int), 1, file);
			for(d=0; d<displaysNumber; d++)
			{
				MaterialDisplay * display = subMesh->getDisplay(d);
				
				PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
				unsigned int begin = display->getBegin();
				unsigned int size = display->getSize();
				Material * material = display->getMaterial();
				CULL_MODES cullMode = display->getCullMode();
				
				int materialId = getMaterialId(mesh, material);
				
				// data
				M_fwrite(&primitiveType, sizeof(PRIMITIVE_TYPES), 1, file);
				M_fwrite(&begin, sizeof(int), 1, file);
				M_fwrite(&size, sizeof(int), 1, file);
				M_fwrite(&materialId, sizeof(int), 1, file);
				M_fwrite(&cullMode, sizeof(CULL_MODES), 1, file);
			}
		}
	}
	

	M_fclose(file);
	return true;
}


// Armature anim bin export
bool exportArmatureAnimBin(const char * filename, ArmatureAnim * anim)
{
	int version = 1;
	
	
	// create file
	File * file = M_fopen(filename, "wb");
	if(! file)
	{
		fprintf(stderr, "Error : can't create file %s\n", filename);
		return false;
	}
	
	
	// header
	M_fwrite(M_AA_HEADER, sizeof(char), 8, file);
	
	// version
	M_fwrite(&version, sizeof(int), 1, file);
	
	
	// bones
	unsigned int b, bonesAnimNumber = anim->getBonesAnimNumber();
	Object3dAnim * bonesAnim = anim->getBonesAnim();
	
	M_fwrite(&bonesAnimNumber, sizeof(int), 1, file);
	for(b=0; b<bonesAnimNumber; b++)
	{
		Object3dAnim * objAnim = &(bonesAnim[b]);
	
		unsigned int positionKeysNumber = objAnim->getPositionKeysNumber();
		unsigned int scaleKeysNumber = objAnim->getScaleKeysNumber();
		unsigned int rotationKeysNumber = objAnim->getRotationKeysNumber();
		
		Key * positionKeys = objAnim->getPositionKeys();
		Key * scaleKeys = objAnim->getScaleKeys();
		Key * rotationKeys = objAnim->getRotationKeys();
	
		writeKeys(file, positionKeys, M_VARIABLE_VEC3, positionKeysNumber);
		writeKeys(file, scaleKeys, M_VARIABLE_VEC3, scaleKeysNumber);
		writeKeys(file, rotationKeys, M_VARIABLE_QUAT, rotationKeysNumber);
	}
	
	
	M_fclose(file);
	return true;
}


// Textures anim bin export
bool exportTexturesAnimBin(const char * filename, TexturesAnim * anim)
{
	int version = 1;
	
	
	// create file
	File * file = M_fopen(filename, "wb");
	if(! file)
	{
		fprintf(stderr, "Error : can't create file %s\n", filename);
		return false;
	}
	
	
	// header
	M_fwrite(M_TA_HEADER, sizeof(char), 8, file);
	
	// version
	M_fwrite(&version, sizeof(int), 1, file);
	
	
	// textures
	unsigned int t, texturesAnimNumber = anim->getTexturesAnimNumber();
	TextureAnim * texturesAnim = anim->getTexturesAnim();
	
	M_fwrite(&texturesAnimNumber, sizeof(int), 1, file);
	for(t=0; t<texturesAnimNumber; t++)
	{
		TextureAnim * texAnim = &(texturesAnim[t]);
		
		unsigned int translateKeysNumber = texAnim->getTranslateKeysNumber();
		unsigned int scaleKeysNumber = texAnim->getScaleKeysNumber();
		unsigned int rotationKeysNumber = texAnim->getRotationKeysNumber();
		
		Key * translateKeys = texAnim->getTranslateKeys();
		Key * scaleKeys = texAnim->getScaleKeys();
		Key * rotationKeys = texAnim->getRotationKeys();
		
		writeKeys(file, translateKeys, M_VARIABLE_VEC2, translateKeysNumber);
		writeKeys(file, scaleKeys, M_VARIABLE_VEC2, scaleKeysNumber);
		writeKeys(file, rotationKeys, M_VARIABLE_FLOAT, rotationKeysNumber);
	}
	
	
	M_fclose(file);
	return true;	
}


// Materials anim bin export
bool exportMaterialsAnimBin(const char * filename, MaterialsAnim * anim)
{
	int version = 1;
	
	
	// create file
	File * file = M_fopen(filename, "wb");
	if(! file)
	{
		fprintf(stderr, "Error : can't create file %s\n", filename);
		return false;
	}
	
	
	// header
	M_fwrite(M_MA_HEADER, sizeof(char), 8, file);
	
	// version
	M_fwrite(&version, sizeof(int), 1, file);
	
	
	// materials
	unsigned int m, materialsAnimNumber = anim->getMaterialsAnimNumber();
	MaterialAnim * materialsAnim = anim->getMaterialsAnim();
	
	M_fwrite(&materialsAnimNumber, sizeof(int), 1, file);
	for(m=0; m<materialsAnimNumber; m++)
	{
		MaterialAnim * matAnim = &(materialsAnim[m]);
		
		unsigned int opacityKeysNumber = matAnim->getOpacityKeysNumber();
		unsigned int shininessKeysNumber = matAnim->getShininessKeysNumber();
		unsigned int customValueKeysNumber = matAnim->getCustomValueKeysNumber();
		unsigned int diffuseKeysNumber = matAnim->getDiffuseKeysNumber();
		unsigned int specularKeysNumber = matAnim->getSpecularKeysNumber();
		unsigned int emitKeysNumber = matAnim->getEmitKeysNumber();
		unsigned int customColorKeysNumber = matAnim->getCustomColorKeysNumber();
		
		Key * opacityKeys = matAnim->getOpacityKeys();
		Key * shininessKeys = matAnim->getShininessKeys();
		Key * customValueKeys = matAnim->getCustomValueKeys();
		Key * diffuseKeys = matAnim->getDiffuseKeys();
		Key * specularKeys = matAnim->getSpecularKeys();
		Key * emitKeys = matAnim->getEmitKeys();
		Key * customColorKeys = matAnim->getCustomColorKeys();
		
		writeKeys(file, opacityKeys, M_VARIABLE_FLOAT, opacityKeysNumber);
		writeKeys(file, shininessKeys, M_VARIABLE_FLOAT, shininessKeysNumber);
		writeKeys(file, customValueKeys, M_VARIABLE_FLOAT, customValueKeysNumber);
		writeKeys(file, diffuseKeys, M_VARIABLE_VEC3, diffuseKeysNumber);
		writeKeys(file, specularKeys, M_VARIABLE_VEC3, specularKeysNumber);
		writeKeys(file, emitKeys, M_VARIABLE_VEC3, emitKeysNumber);
		writeKeys(file, customColorKeys, M_VARIABLE_VEC3, customColorKeysNumber);
	}
	
	
	M_fclose(file);
	return true;	
}
