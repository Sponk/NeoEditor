//========================================================================
//  Copyright (c) 2013 Anael Seghezzi <www.maratis3d.com>
//	Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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

// done using code from assimp samples : http://assimp.sourceforge.net/

#ifndef ANDROID

#include <NeoEngine.h>

#include "AssimpMeshLoader.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <MeshSave.h>

using namespace Neo;

static NeoEngine * engine = NeoEngine().getInstance();
static Level * level = NULL;

struct BoneData
{
	unsigned int id;
	float weight;
};

struct AssimpSkinData
{
	vector<BoneData> bones;
};

struct NodeInfos
{
	unsigned int nbKeyPos;
	unsigned int nbKeyRot;
	unsigned int nbKeyScale;
};


static void countMeshs(const aiScene * scene, const aiNode * nd, unsigned int * count)
{
	for(unsigned int m=0; m<nd->mNumMeshes; m++)
	{
		const aiMesh * nodeMesh = scene->mMeshes[nd->mMeshes[m]];
		if(nodeMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
			(*count)++;
	}
	
	for(unsigned int n=0; n<nd->mNumChildren; n++)
		countMeshs(scene, nd->mChildren[n], count);
}


static void countNodes(const aiScene * scene, const aiNode * nd, unsigned int * bonesCount)
{
	(*bonesCount)++;
	
	for(unsigned int n=0; n<nd->mNumChildren; n++)
		countNodes(scene, nd->mChildren[n], bonesCount);
}


static void createArmature(const aiScene * scene, const aiNode * nd, Armature * armature, OBone * parent, const Matrix4x4 & parentMatrix)
{	
	aiMatrix4x4 nodeMat = nd->mTransformation;
	aiTransposeMatrix4(&nodeMat);
	
	Matrix4x4 matrix = Matrix4x4((float*)&nodeMat);
	Matrix4x4 globalMatrix = parentMatrix * matrix;
	
	if(parent == NULL)
		matrix = globalMatrix;

	OBone * bone = armature->addNewBone();
	bone->setName(nd->mName.data);
	bone->linkTo(parent);
	
	Vector3 pos = matrix.getTranslationPart();
	Vector3 rot = matrix.getEulerAngles();
	Vector3 scale = matrix.getScale();
		
	bone->setPosition(pos);
	bone->setEulerRotation(rot);
	bone->setScale(scale);
	
	// childrens
	for(unsigned int n=0; n<nd->mNumChildren; n++)
		createArmature(scene, nd->mChildren[n], armature, bone, globalMatrix);
}

static void initBones(const aiScene * scene, const aiMesh * nodeMesh, Mesh * mesh, SubMesh * subMesh)
{
	Armature * armature = mesh->getArmature();
	
	BoneData bdata;
	map<unsigned int, AssimpSkinData> skinDatas;
	
	
	// bones
	for(unsigned int i=0; i<nodeMesh->mNumBones; i++)
	{
		aiBone * nodeBone = nodeMesh->mBones[i];
		
		unsigned int boneId;
		if(! armature->getBoneId(nodeBone->mName.data, &boneId))
			continue;
		
		OBone * bone = armature->getBone(boneId);
		
		aiMatrix4x4 offsetMat = nodeBone->mOffsetMatrix;
		aiTransposeMatrix4(&offsetMat);
		Matrix4x4 matrix = (*bone->getMatrix()) * Matrix4x4((float*)&offsetMat);
		
		
		// pose skinning
		Vector3 * vertices = subMesh->getVertices();
		Vector3 * normals = subMesh->getNormals();
		Vector3 * tangents = subMesh->getTangents();
		
		unsigned int w;
		for(w=0; w<nodeBone->mNumWeights; w++)
		{
			unsigned int vid = nodeBone->mWeights[w].mVertexId;
			float weight = nodeBone->mWeights[w].mWeight; 
			
			bdata.id = boneId;
			bdata.weight = weight;
			skinDatas[vid].bones.push_back(bdata);

			if(skinDatas[vid].bones.size() == 1)
			{
				if(vertices) vertices[vid] = Vector3(0, 0, 0);
				if(normals) normals[vid] = Vector3(0, 0, 0);
				if(tangents) tangents[vid] = Vector3(0, 0, 0);
			}
			
			if(vertices)
				vertices[vid] += matrix * Vector3(nodeMesh->mVertices[vid].x, nodeMesh->mVertices[vid].y, nodeMesh->mVertices[vid].z) * weight;
			
			if(normals)
				normals[vid] += matrix.getRotatedVector3(Vector3(nodeMesh->mNormals[vid].x, nodeMesh->mNormals[vid].y, nodeMesh->mNormals[vid].z)) * weight;
			
			if(tangents)
				tangents[vid] += matrix.getRotatedVector3(Vector3(nodeMesh->mTangents[vid].x, nodeMesh->mTangents[vid].y, nodeMesh->mTangents[vid].z)) * weight;
		}
	}
	
	
	// alloc skin
	unsigned int skinSize = skinDatas.size();
	if(skinSize > 0)
	{
		SkinData * skin = subMesh->createSkinData();
		SkinPoint * skinPoints = skin->allocPoints(skinSize);
		
		map<unsigned int, AssimpSkinData>::iterator
			mit (skinDatas.begin()),
			mend(skinDatas.end());
		
		unsigned int p = 0;
		for(; mit!=mend; ++mit)
		{
			unsigned int vertexId = mit->first;
			AssimpSkinData * sdata = &mit->second;
			
			unsigned int b, bSize = sdata->bones.size();
			
			if(skinPoints[p].allocateBonesLinks(bSize))
			{
				skinPoints[p].setVertexId(vertexId);
				
				unsigned short * ids = skinPoints[p].getBonesIds();
				float * weights = skinPoints[p].getBonesWeights();
				
				for(b=0; b<bSize; b++)
				{
					ids[b] = sdata->bones[b].id;
					weights[b] = sdata->bones[b].weight;
				}
			}
			
			p++;
		}
	}
}


static void createSubMesh(const aiScene * scene, const aiNode * nd, Mesh * mesh, SubMesh * subMeshs, unsigned int * count, const Matrix4x4 & parentMatrix)
{
	aiMatrix4x4 nodeMat = nd->mTransformation;
	aiTransposeMatrix4(&nodeMat);
	
	Matrix4x4 matrix = parentMatrix * Matrix4x4((float*)&nodeMat);
	
	unsigned int m;
	for(m=0; m<nd->mNumMeshes; m++)
	{
		unsigned int i;
		const aiMesh * nodeMesh = scene->mMeshes[nd->mMeshes[m]];
		if(nodeMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
			continue;
		
		
		// current sub mesh
		SubMesh * subMesh = &(subMeshs[*count]);
	
		
		// vertices
		Vector3 * vertices = subMesh->allocVertices(nodeMesh->mNumVertices);
		for(i=0; i<nodeMesh->mNumVertices; i++)
			vertices[i] = matrix * Vector3(nodeMesh->mVertices[i].x, nodeMesh->mVertices[i].y, nodeMesh->mVertices[i].z);
		
		// normals
		if(nodeMesh->mNormals)
		{
			Vector3 * normals = subMesh->allocNormals(nodeMesh->mNumVertices);
			for(i=0; i<nodeMesh->mNumVertices; i++)
				normals[i] = matrix.getRotatedVector3(Vector3(nodeMesh->mNormals[i].x, nodeMesh->mNormals[i].y, nodeMesh->mNormals[i].z)).getNormalized();
		}
		
		// tangents
		if(nodeMesh->mTangents)
		{
			Vector3 * tangents = subMesh->allocTangents(nodeMesh->mNumVertices);
			for(i=0; i<nodeMesh->mNumVertices; i++)
				tangents[i] = matrix.getRotatedVector3(Vector3(nodeMesh->mTangents[i].x, nodeMesh->mTangents[i].y, nodeMesh->mTangents[i].z)).getNormalized();
		}
		
		// colors
		if(nodeMesh->mColors[0])
		{
			Color * colors = subMesh->allocColors(nodeMesh->mNumVertices);
			for(i=0; i<nodeMesh->mNumVertices; i++)
				colors[i] = Color(nodeMesh->mColors[0][i].r, nodeMesh->mColors[0][i].g, nodeMesh->mColors[0][i].b, nodeMesh->mColors[0][i].a);
		}
		
		// uvcoords
		{
			unsigned int t, nb_texLayers = 0;
			for(t=0; t<AI_MAX_NUMBER_OF_TEXTURECOORDS; t++)
			{
				if(nodeMesh->mTextureCoords[t])
					nb_texLayers++;
			}
			
			Vector2 * uvCoords = subMesh->allocTexCoords(nodeMesh->mNumVertices*nb_texLayers);
	
			nb_texLayers = 0;
			for(t=0; t<AI_MAX_NUMBER_OF_TEXTURECOORDS; t++)
			{
				if(nodeMesh->mTextureCoords[t])
				{
					unsigned int offset = nodeMesh->mNumVertices*nb_texLayers;
					subMesh->setMapChannelOffset(t, offset);
					
					for(i=0; i<nodeMesh->mNumVertices; i++){
						uvCoords[offset+i] = Vector2(nodeMesh->mTextureCoords[t][i].x, 1-nodeMesh->mTextureCoords[t][i].y);
					}
					
					nb_texLayers++;
				}
			}
		}
		
		// indices
		{
			unsigned int f;
			
			if(nodeMesh->mNumVertices < 65536)
			{
				unsigned short * indices = (unsigned short *)subMesh->allocIndices(nodeMesh->mNumFaces*3, VAR_USHORT);
				for(f=0; f<nodeMesh->mNumFaces; f++)
				{
					aiFace * face = &nodeMesh->mFaces[f];
					for(i=0; i<3; i++){
						indices[f*3+i] = face->mIndices[i];
					}
				}
			}
			else	
			{
				unsigned int * indices = (unsigned int *)subMesh->allocIndices(nodeMesh->mNumFaces*3, VAR_UINT);
				for(f=0; f<nodeMesh->mNumFaces; f++)
				{
					aiFace * face = &nodeMesh->mFaces[f];
					for(i=0; i<3; i++){
						indices[f*3+i] = face->mIndices[i];
					}
				}
			}
		}
		
		// bones
		initBones(scene, nodeMesh, mesh, subMesh);

		// display
		subMesh->allocDisplays(1);
		MaterialDisplay * display = subMesh->addNewDisplay(PRIMITIVE_TRIANGLES, 0, subMesh->getIndicesSize());
		display->setMaterial(mesh->getMaterial(nodeMesh->mMaterialIndex));
		display->setMaterialId(nodeMesh->mMaterialIndex);

		// cull mode
		int twosided = 0;
		aiMaterial * mtl = scene->mMaterials[nodeMesh->mMaterialIndex];
		if(AI_SUCCESS == aiGetMaterialInteger(mtl, AI_MATKEY_TWOSIDED, &twosided))
		{
			if(twosided != 0)
				display->setCullMode(CULL_NONE);
		}
		
		// bounding box
		subMesh->getBoundingBox()->initFromPoints(vertices, subMesh->getVerticesSize());
		
		(*count)++;
	}
	
	
	for(unsigned int n=0; n<nd->mNumChildren; n++)
		createSubMesh(scene, nd->mChildren[n], mesh, subMeshs, count, matrix);
}


static int getMaratisTick(double tick, double tickPerSec)
{
	if(tickPerSec == 0)
		return (int)tick;
	
	return (int)((tick/tickPerSec)*60.0);
}


void readAssimpMesh(const char * filename, const aiScene * scene, const aiNode * node, Mesh * mesh, const char * meshRep, bool rotate90)
{
	unsigned int i;
	char globalPath[256];
		
	// clear mesh
	mesh->clear();
		
	// textures
	unsigned int nb_textures = 0;
	for(i=0; i<scene->mNumMaterials; i++)
	{
		aiMaterial * mtl = scene->mMaterials[i];
		
		aiString path;
		
		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path))
			nb_textures++;
		
		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &path))
			nb_textures++;
		
		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &path))
			nb_textures++;
		
		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_EMISSIVE, 0, &path))
			nb_textures++;
	}
	
	mesh->allocTextures(nb_textures);
	
	// materials
	mesh->allocMaterials(scene->mNumMaterials);
	for(i=0; i<scene->mNumMaterials; i++)
	{
		aiMaterial * mtl = scene->mMaterials[i];
		Material * material = mesh->addNewMaterial();
		
		float value;
		aiColor4D color;
		
		material->setType(1);
		
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
			material->setDiffuse(Vector3(color.r, color.g, color.b));
		
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
			material->setSpecular(Vector3(color.r, color.g, color.b));
		
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &color))
			material->setEmit(Vector3(color.r, color.g, color.b));
	
		if(AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_OPACITY, &value))
		   material->setOpacity(value);
		
		if(AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &value))
		{
			material->setShininess(value*0.25f); // Need to quarter values since Assimp multiplies with 4 for some reason.
		}

		// blend
		{
			int blendMode;
			if(AI_SUCCESS == aiGetMaterialInteger(mtl, AI_MATKEY_BLEND_FUNC, &blendMode))
			{
				switch(blendMode)
				{
					default:
					case aiBlendMode_Default:
						material->setBlendMode(BLENDING_ALPHA);
						break;
					case aiBlendMode_Additive:
						material->setBlendMode(BLENDING_ADD);
						break;
				}
			}
			else if(material->getOpacity() < 1.0)
				material->setBlendMode(BLENDING_ALPHA);
			else material->setBlendMode(BLENDING_NONE);
		}

		// textures
		{
			aiString path;
			aiTextureMapping mapping;
			unsigned int uvindex;
			float blend;
			aiTextureOp op;
			aiTextureMapMode mapmode;
			

			if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &path, &mapping, &uvindex, &blend, &op, &mapmode))
			{
				material->allocTexturesPass(4);
				
				getGlobalFilename(globalPath, meshRep, path.C_Str());
				TextureRef * texRef = level->loadTexture(globalPath, true);
				Texture * texture = mesh->addNewTexture(texRef);
				if(mapmode == aiTextureMapMode_Clamp){
					texture->setUWrapMode(WRAP_CLAMP);
					texture->setVWrapMode(WRAP_CLAMP);
				}
				
				material->addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}
		
			if(AI_SUCCESS == mtl->GetTexture(aiTextureType_SPECULAR, 0, &path, &mapping, &uvindex, &blend, &op, &mapmode))
			{
				getGlobalFilename(globalPath, meshRep, path.C_Str());
				TextureRef * texRef = level->loadTexture(globalPath, true);
				Texture * texture = mesh->addNewTexture(texRef);
				if(mapmode == aiTextureMapMode_Clamp){
					texture->setUWrapMode(WRAP_CLAMP);
					texture->setVWrapMode(WRAP_CLAMP);
				}
				
				while(material->getTexturesPassNumber() < 1)
					material->addTexturePass(NULL, TEX_COMBINE_MODULATE, 0);
				
				material->addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}
		
			if(AI_SUCCESS == mtl->GetTexture(aiTextureType_NORMALS, 0, &path, &mapping, &uvindex, &blend, &op, &mapmode))
			{
				getGlobalFilename(globalPath, meshRep, path.C_Str());
				TextureRef * texRef = level->loadTexture(globalPath, true);
				Texture * texture = mesh->addNewTexture(texRef);
				if(mapmode == aiTextureMapMode_Clamp){
					texture->setUWrapMode(WRAP_CLAMP);
					texture->setVWrapMode(WRAP_CLAMP);
				}
				
				while(material->getTexturesPassNumber() < 2)
					material->addTexturePass(NULL, TEX_COMBINE_MODULATE, 0);
					
				material->addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}
		
			if(AI_SUCCESS == mtl->GetTexture(aiTextureType_EMISSIVE, 0, &path, &mapping, &uvindex, &blend, &op, &mapmode))
			{
				getGlobalFilename(globalPath, meshRep, path.C_Str());
				TextureRef * texRef = level->loadTexture(globalPath, true);
				Texture * texture = mesh->addNewTexture(texRef);
				if(mapmode == aiTextureMapMode_Clamp){
					texture->setUWrapMode(WRAP_CLAMP);
					texture->setVWrapMode(WRAP_CLAMP);
				}
				
				while(material->getTexturesPassNumber() < 3)
					material->addTexturePass(NULL, TEX_COMBINE_MODULATE, 0);
				
				material->addTexturePass(texture, TEX_COMBINE_MODULATE, uvindex);
			}
		}
	}
	

	// subMeshs
	unsigned int nb_subMeshs = 0, nb_bones = 0;
	countMeshs(scene, node, &nb_subMeshs);
	countNodes(scene, scene->mRootNode, &nb_bones);
	
	if(nb_subMeshs > 0)
	{
		Matrix4x4 rootMatrix;
		if(rotate90)
			rootMatrix.rotate(Vector3(1, 0, 0), 90);
		
		// create armature
		if(nb_bones > 0)
		{
			Armature * armature = mesh->createArmature();
			
			armature->allocBones(nb_bones);
			createArmature(scene, scene->mRootNode, armature, NULL, rootMatrix);
			armature->constructBonesInversePoseMatrix();
		}
		
		// create subMeshs
		SubMesh * subMeshs = mesh->allocSubMeshs(nb_subMeshs);
		
		unsigned int count = 0;
		createSubMesh(scene, node, mesh, subMeshs, &count, rootMatrix);
	}
	
	
	// do we need armature
	{
		bool skinning = false;
		unsigned int s, sSize = mesh->getSubMeshsNumber();
		for(s=0; s<sSize; s++)
		{
			if(mesh->getSubMeshs()[s].getSkinData())
			{
				skinning = true;
				break;
			}
		}
		
		if(! skinning)
			mesh->clearArmature();
	}
	
	
	// animation
	if(mesh->getArmature() && scene->mNumAnimations > 0)
	{
		Armature * armature = mesh->getArmature();
		
		
		char maaName[256];
		sprintf(maaName, "%s.maa", filename);
		
		
		ArmatureAnim * armatureAnim = ArmatureAnim::getNew();
		
		ArmatureAnimRef * armatureAnimRef = ArmatureAnimRef::getNew(armatureAnim, maaName);
		level->getArmatureAnimManager()->addRef(armatureAnimRef);
		mesh->setArmatureAnimRef(armatureAnimRef);
		
		AnimRange * animRanges = mesh->allocAnimsRanges(scene->mNumAnimations);
		
		
		// armature anim
		if(armatureAnim)
		{
			unsigned int a, aSize = scene->mNumAnimations;
		
			Object3dAnim * bonesAnim = armatureAnim->allocBonesAnim(armature->getBonesNumber());
		
			
			// node infos (for multiple anims merging)
			map <string, NodeInfos> nodesInfos;
			for(a=0; a<aSize; a++)
			{
				aiAnimation * anim = scene->mAnimations[a];
			
				unsigned int c, cSize;
				for(c=0; c<anim->mNumChannels; c++)
				{
					aiNodeAnim * channel = anim->mChannels[c];
					NodeInfos * infos = &(nodesInfos[channel->mNodeName.data]);
					
					infos->nbKeyPos += channel->mNumPositionKeys;
					infos->nbKeyRot	+= channel->mNumRotationKeys;
					infos->nbKeyScale += channel->mNumScalingKeys;
				}
			}
			
			
			// alloc keys
			{
				map<string, NodeInfos>::iterator
					mit (nodesInfos.begin()),
					mend(nodesInfos.end());

				for(; mit!=mend; mit++)
				{
					unsigned int boneId;
					if(armature->getBoneId(mit->first.c_str(), &boneId))
					{
						Object3dAnim * boneAnim = &(bonesAnim[boneId]);
					
						if(mit->second.nbKeyPos > 0)
							boneAnim->allocPositionKeys(mit->second.nbKeyPos);
						
						if(mit->second.nbKeyRot > 0)
							boneAnim->allocRotationKeys(mit->second.nbKeyRot);
							
						if(mit->second.nbKeyScale > 0)
							boneAnim->allocScaleKeys(mit->second.nbKeyScale);
							
						mit->second.nbKeyPos = 0;
						mit->second.nbKeyRot = 0;
						mit->second.nbKeyScale = 0;
					}
				}
			}
			
			
			// anims
			int prevT = 0;
			for(a=0; a<aSize; a++)
			{
				aiAnimation * anim = scene->mAnimations[a];
			
				animRanges[a].start = prevT;
				animRanges[a].end = prevT + getMaratisTick(anim->mDuration, anim->mTicksPerSecond);
				
				
				unsigned int c, cSize;
				for(c=0; c<anim->mNumChannels; c++)
				{
					aiNodeAnim * channel = anim->mChannels[c];
					NodeInfos * infos = &(nodesInfos[channel->mNodeName.data]);

				
					unsigned int boneId;
					if(armature->getBoneId(channel->mNodeName.data, &boneId))
					{
						unsigned int k;
						
						Object3dAnim * boneAnim = &(bonesAnim[boneId]);
						
						Key * posKeys = boneAnim->getPositionKeys();
						Key * rotKeys = boneAnim->getRotationKeys();
						Key * scaleKeys = boneAnim->getScaleKeys();
						
						
						// pos
						for(k=0; k<channel->mNumPositionKeys; k++)
						{
							posKeys[infos->nbKeyPos+k].setT(prevT + getMaratisTick(channel->mPositionKeys[k].mTime, anim->mTicksPerSecond));
							*(posKeys[infos->nbKeyPos+k].createVector3Data()) = Vector3(
								channel->mPositionKeys[k].mValue.x,
								channel->mPositionKeys[k].mValue.y,
								channel->mPositionKeys[k].mValue.z
							);
						}
						
						// rot
						for(k=0; k<channel->mNumRotationKeys; k++)
						{
							rotKeys[infos->nbKeyRot+k].setT(prevT + getMaratisTick(channel->mRotationKeys[k].mTime, anim->mTicksPerSecond));
							*(rotKeys[infos->nbKeyRot+k].createQuaternionData()) = Quaternion(
								channel->mRotationKeys[k].mValue.x,
								channel->mRotationKeys[k].mValue.y,
								channel->mRotationKeys[k].mValue.z,
								channel->mRotationKeys[k].mValue.w
							);
						}
						
						// scale
						for(k=0; k<channel->mNumScalingKeys; k++)
						{
							scaleKeys[infos->nbKeyScale+k].setT(prevT + getMaratisTick(channel->mScalingKeys[k].mTime, anim->mTicksPerSecond));
							*(scaleKeys[infos->nbKeyScale+k].createVector3Data()) = Vector3(
								channel->mScalingKeys[k].mValue.x,
								channel->mScalingKeys[k].mValue.y,
								channel->mScalingKeys[k].mValue.z
							);
						}
						
						
						infos->nbKeyPos += channel->mNumPositionKeys;
						infos->nbKeyRot	+= channel->mNumRotationKeys;
						infos->nbKeyScale += channel->mNumScalingKeys;
					}
				}
				
				prevT = animRanges[a].end + 1;
			}
		}
	}
	
	
	mesh->updateBoundingBox();
}


bool M_loadAssimpMesh(const char * filename, Mesh* mesh)
{
	const aiScene * scene = aiImportFile(filename, 0);
	
	if(! scene)
		return false;

	if(! scene->mRootNode)
		return false;	
	
	// post process
	unsigned int ppFlags = 
		aiProcess_CalcTangentSpace |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes;
		
	if(scene->mNumMeshes > 64)
		ppFlags = ppFlags | aiProcess_OptimizeGraph;
	
	if(! aiApplyPostProcessing(scene, ppFlags))
			return false;
	
	
	// level
	level = engine->getLevel();

	// mesh rep
	char meshRep[256];
	getRepertory(meshRep, filename);

	// source name
	char sourceName[256];
	getLocalFilename(sourceName, meshRep, filename);
	
	// rotate 90
	bool rotate90 = false;
	if(strstr(filename, ".blend") == 0 && strstr(filename, ".stl") == 0)
		rotate90 = true;

	// read assimp root node
	readAssimpMesh(sourceName, scene, scene->mRootNode, mesh, meshRep, rotate90);
	
	aiReleaseImport(scene);
	return true;
}


bool M_importAssimpMeshes(const char * filename)
{
	const aiScene * scene = aiImportFile(filename, 0);
	
	if(! scene)
		return false;

	if(! scene->mRootNode)
		return false;
	
	
	// post process
	unsigned int ppFlags = 
		aiProcess_CalcTangentSpace |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes;
		
	if(scene->mNumMeshes > 64)
		ppFlags = ppFlags | aiProcess_OptimizeGraph;
	
	if(! aiApplyPostProcessing(scene, ppFlags))
			return false;
	
	
	// level
	level = engine->getLevel();
	Scene * curScene = level->getCurrentScene();
	
	// mesh rep
	char meshRep[256];
	getRepertory(meshRep, filename);
	
	// working dir
	const char * workingDirectory = engine->getSystemContext()->getWorkingDirectory();

	// source name
	char sourceName[256];
	getLocalFilename(sourceName, meshRep, filename);	

	// mesh export dir
	char exportDir[256];
	getGlobalFilename(exportDir, workingDirectory, (string("meshs/") + string(sourceName)).c_str());
	if(! isDirectory(exportDir))
	{
		if(! createDirectory(exportDir))
		{
			MLOG_ERROR("impossible to import " << filename);
			aiReleaseImport(scene);
			return false;
		}
	}
	
	
	// rotate 90
	bool rotate90 = false;
	if(strstr(filename, ".blend") == 0 && strstr(filename, ".stl") == 0)
		rotate90 = true;
			
	
	char nodeName[256];
	sprintf(nodeName, "%s.root", sourceName);
	
	char meshName[256];
	getGlobalFilename(meshName, exportDir, "root.mesh");
	
	
	// mesh
	MeshRef * meshRef = level->loadMesh(meshName);
	Mesh * mesh = meshRef->getMesh();

	// read assimp root node
	readAssimpMesh("root", scene, scene->mRootNode, mesh, meshRep, rotate90);
	
	
	// add to scene and save xml mesh
	if(mesh->getSubMeshsNumber() > 0)
	{
		OEntity * entity = curScene->addNewEntity(meshRef);
		entity->setName(nodeName);
		
		// save
		ArmatureAnimRef * maaRef = mesh->getArmatureAnimRef();
		//MMaterialsAnimRef * mmaRef = mesh->getMaterialsAnimRef();
		//MTexturesAnimRef * mtaRef = mesh->getTexturesAnimRef();
		
		xmlMeshSave(meshName, mesh);
		
		if(maaRef)
		{
			if(maaRef->getArmatureAnim())
			{
				getGlobalFilename(meshName, exportDir, "root.maa");
				xmlArmatureAnimSave(meshName, maaRef->getArmatureAnim());
			}
		}
			
		// not supported by assimp
		//xmlTexturesAnimSave(const char * filename, MTexturesAnim * anim);
		//xmlMaterialsAnimSave(const char * filename, MMaterialsAnim * anim);
	}
	
	
	aiReleaseImport(scene);
	return true;
}

#endif
