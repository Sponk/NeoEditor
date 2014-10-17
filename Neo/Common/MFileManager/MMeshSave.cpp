/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MMeshSave.cpp
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
#include "MMeshSave.h"
#include "MXmlCommon.h"



// tools
static void writeKey(MFile * file, MKey * key, M_VARIABLE_TYPE type)
{
	openAttributeNode(file, "k", 3);

	int t = key->getT();
	writeInt(file, "t", t, " ");
	
	switch(type)
	{
		case M_VARIABLE_FLOAT:
		{
			float * data = (float *)key->getData();
			writeFloat(file, "value", *data);
			break;
		}
		case M_VARIABLE_VEC2:
		{
			MVector2 * data = (MVector2 *)key->getData();
			writeFloat(file, "x", data->x, " ");
			writeFloat(file, "y", data->y);
			break;
		}
		case M_VARIABLE_VEC3:
		{
			MVector3 * data = (MVector3 *)key->getData();
			writeFloat(file, "x", data->x, " ");
			writeFloat(file, "y", data->y, " ");
			writeFloat(file, "z", data->z);
			break;
		}
		case M_VARIABLE_VEC4:
		{
			MVector4 * data = (MVector4 *)key->getData();
			writeFloat(file, "x", data->x, " ");
			writeFloat(file, "y", data->y, " ");
			writeFloat(file, "z", data->z, " ");
			writeFloat(file, "w", data->w);
			break;
		}
		case M_VARIABLE_QUAT:
		{
			MQuaternion * data = (MQuaternion *)key->getData();
			MVector3 euler = data->getEulerAngles();
			writeFloat(file, "x", euler.x, " ");
			writeFloat(file, "y", euler.y, " ");
			writeFloat(file, "z", euler.z);
			break;
		}
	}
	
	closeAttributeNode(file, "\n");
}

static void writeKeys(MFile * file, const char * name, MKey * keys, M_VARIABLE_TYPE type, unsigned int keysNumber)
{
	if(keysNumber > 0)
	{
		openAttributeNode(file, name, 2);
		writeInt(file, "num", keysNumber);
		closeNodeAttributes(file, "\n");

		unsigned int k;
		for(k=0; k<keysNumber; k++)
			writeKey(file, &(keys[k]), type);
			
		closeNode(file, name, 2, "\n");
	}
}


static int getTextureId(MMesh * mesh, MTexture * texture)
{
	unsigned int t, texturesNumber = mesh->getTexturesNumber();
	for(t=0; t<texturesNumber; t++)
	{
		if(mesh->getTexture(t) == texture)
			return t;
	}
	
	return -1;
}

static int getMaterialId(MMesh * mesh, MMaterial * material)
{
	unsigned int m, materialsNumber = mesh->getMaterialsNumber();
	for(m=0; m<materialsNumber; m++)
	{
		if(mesh->getMaterial(m) == material)
			return m;
	}
	
	return -1;
}


// Mesh export
bool xmlMeshSave(const char * filename, MMesh * mesh)
{
	char rep[256];
	
	
	// create file
	MFile * file = M_fopen(filename, "wt");
	if(! file)
	{
		MLOG_ERROR("can't create file : " << filename);
		return false;
	}
	
	
	// get file directory
	getRepertory(rep, filename);
	
	
	// Maratis node
	openNode(file, "Maratis version=\"3.0\"", 0, "\n\n");
	
	// Mesh node
	openNode(file, "Mesh", 0, "\n\n");
	

	// Animation
	{
		// anim refs
		MArmatureAnimRef * armatureAnimRef = mesh->getArmatureAnimRef();
		MTexturesAnimRef * texturesAnimRef = mesh->getTexturesAnimRef();
		MMaterialsAnimRef * materialsAnimRef = mesh->getMaterialsAnimRef();
		
		if(armatureAnimRef || texturesAnimRef || materialsAnimRef)
		{
			// Animation node
			openNode(file, "Animation", 0, "\n");
			
			
			// armature anim ref
			if(armatureAnimRef)
			{
				openAttributeNode(file, "ArmatureAnim", 1);
				writeFilename(file, "file", armatureAnimRef->getFilename(), rep);
				closeAttributeNode(file, "\n");
			}
			
			// textures anim ref
			if(texturesAnimRef)
			{
				openAttributeNode(file, "TextureAnim", 1);
				writeFilename(file, "file", texturesAnimRef->getFilename(), rep);
				closeAttributeNode(file, "\n");
			}
			
			// materials anim ref
			if(materialsAnimRef)
			{
				openAttributeNode(file, "MaterialAnim", 1);
				writeFilename(file, "file", materialsAnimRef->getFilename(), rep);
				closeAttributeNode(file, "\n");
			}
			
			// anims ranges
			unsigned int a, animsRangesNumber = mesh->getAnimsRangesNumber();
			MAnimRange * animsRanges = mesh->getAnimsRanges();
			
			if(animsRangesNumber > 0)
			{
				openAttributeNode(file, "Anims", 1);
				writeInt(file, "num", animsRangesNumber);
				closeNodeAttributes(file, "\n");
	
				for(a=0; a<animsRangesNumber; a++)
				{
					openAttributeNode(file, "anim", 2);
					writeInt(file, "id", a, " ");
					writeInt(file, "start", animsRanges[a].start, " ");
					writeInt(file, "end", animsRanges[a].end, " ");
					writeInt(file, "loops", animsRanges[a].loops);
					closeAttributeNode(file, "\n");
				}
				
				closeNode(file, "Anims", 1, "\n");
			}
			
			
			// close Animation node
			closeNode(file, "Animation", 0, "\n\n");
		}
	}
	
	
	// Textures
	{
		unsigned int t, texturesNumber = mesh->getTexturesNumber();
		if(texturesNumber > 0)
		{
			// Textures node
			openAttributeNode(file, "Textures", 0);
			writeInt(file, "num", texturesNumber);
			closeNodeAttributes(file, "\n");
			
			
			for(t=0; t<texturesNumber; t++)
			{
				MTexture * texture = mesh->getTexture(t);
				
				MTextureRef * textureRef = texture->getTextureRef();
				M_TEX_GEN_MODES genMode = texture->getGenMode();
				M_WRAP_MODES UWrapMode = texture->getUWrapMode();
				M_WRAP_MODES VWrapMode = texture->getVWrapMode();
				MVector2 texTranslate = texture->getTexTranslate();
				MVector2 texScale = texture->getTexScale();
				float texRotate = texture->getTexRotate();
				
				
				openAttributeNode(file, "Texture", 1);
				writeInt(file, "id", t);
				closeNodeAttributes(file, "\n");
				
		
				// texture ref
				if(textureRef)
				{
					openAttributeNode(file, "image", 2);
					writeFilename(file, "filename", textureRef->getFilename(), rep, " ");
					(textureRef->isMipmapEnabled()) ? writeString(file, "mipmap", "1") : writeString(file, "mipmap", "0");
					closeAttributeNode(file, "\n");
				}
				
				// data
				openAttributeNode(file, "mapMode", 2);
				writeInt(file, "value", genMode+1);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "tile", 2);
				(UWrapMode == M_WRAP_CLAMP) ? writeString(file, "u", "clamp", " ") : writeString(file, "u", "repeat", " ");
				(VWrapMode == M_WRAP_CLAMP) ? writeString(file, "v", "clamp") : writeString(file, "v", "repeat");
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "translate", 2);
				writeFloat(file, "x", texTranslate.x, " ");
				writeFloat(file, "y", texTranslate.y);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "scale", 2);
				writeFloat(file, "x", texScale.x, " ");
				writeFloat(file, "y", texScale.y);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "rotate", 2);
				writeFloat(file, "angle", texRotate);
				closeAttributeNode(file, "\n");

				
				closeNode(file, "Texture", 1, "\n");
			}
			
			
			// close Textures node
			closeNode(file, "Textures", 0, "\n\n");
		}
	}
	
	
	// Materials
	{
		unsigned int m, materialsNumber = mesh->getMaterialsNumber();
		
		if(materialsNumber > 0)
		{
			// Materials node
			openAttributeNode(file, "Materials", 0);
			writeInt(file, "num", materialsNumber);
			closeNodeAttributes(file, "\n");
			
			
			for(m=0; m<materialsNumber; m++)
			{
				MMaterial * material = mesh->getMaterial(m);
				
				int type = material->getType();
				float opacity = material->getOpacity();
				float shininess = material->getShininess();
				float customValue = material->getCustomValue();
				M_BLENDING_MODES blendMode = material->getBlendMode();
				MVector3 emit = material->getEmit();
				MVector3 diffuse = material->getDiffuse();
				MVector3 specular = material->getSpecular();
				MVector3 customColor = material->getCustomColor();
				MFXRef * FXRef = material->getFXRef();
				MFXRef * ZFXRef = material->getZFXRef();
				
				
				openAttributeNode(file, "Material", 1);
				writeInt(file, "id", m, " ");
				writeInt(file, "type", type);
				closeNodeAttributes(file, "\n");
				
				
				// data
				openAttributeNode(file, "blend", 2);
				switch(blendMode)
				{
					default:
						writeInt(file, "type", 0);
						break;
					case M_BLENDING_ALPHA:
						writeInt(file, "type", 2);
						break;
					case M_BLENDING_ADD:
						writeInt(file, "type", 3);
						break;
					case M_BLENDING_PRODUCT:
						writeInt(file, "type", 4);
						break;
				}
				closeAttributeNode(file, "\n");

				openAttributeNode(file, "opacity", 2);
				writeFloat(file, "value", opacity);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "shininess", 2);
				writeFloat(file, "value", shininess);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "customValue", 2);
				writeFloat(file, "value", customValue);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "diffuseColor", 2);
				writeFloat(file, "r", diffuse.x, " ");
				writeFloat(file, "g", diffuse.y, " ");
				writeFloat(file, "b", diffuse.z);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "specularColor", 2);
				writeFloat(file, "r", specular.x, " ");
				writeFloat(file, "g", specular.y, " ");
				writeFloat(file, "b", specular.z);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "emitColor", 2);
				writeFloat(file, "r", emit.x, " ");
				writeFloat(file, "g", emit.y, " ");
				writeFloat(file, "b", emit.z);
				closeAttributeNode(file, "\n");
				
				openAttributeNode(file, "customColor", 2);
				writeFloat(file, "r", customColor.x, " ");
				writeFloat(file, "g", customColor.y, " ");
				writeFloat(file, "b", customColor.z);
				closeAttributeNode(file, "\n");
				
				
				// FX ref
				if(FXRef)
				{
					MShaderRef * vertShadRef = FXRef->getVertexShaderRef();
					MShaderRef * pixShadRef = FXRef->getPixelShaderRef();
					
					openAttributeNode(file, "vertexShader", 2);
					writeFilename(file, "file", vertShadRef->getFilename(), rep);
					closeAttributeNode(file, "\n");
					
					openAttributeNode(file, "fragmentShader", 2);
					writeFilename(file, "file", pixShadRef->getFilename(), rep);
					closeAttributeNode(file, "\n");
				}
				
				// Z FX ref
				if(ZFXRef)
				{
					MShaderRef * vertShadRef = ZFXRef->getVertexShaderRef();
					MShaderRef * pixShadRef = ZFXRef->getPixelShaderRef();
					
					openAttributeNode(file, "ZVertexShader", 2);
					writeFilename(file, "file", vertShadRef->getFilename(), rep);
					closeAttributeNode(file, "\n");
					
					openAttributeNode(file, "ZFragmentShader", 2);
					writeFilename(file, "file", pixShadRef->getFilename(), rep);
					closeAttributeNode(file, "\n");
				}
				
				
				// textures pass
				unsigned int t, texturesPassNumber = material->getTexturesPassNumber();
				if(texturesPassNumber > 0)
				{
					openAttributeNode(file, "TexturesPass", 2);
					writeInt(file, "num", texturesPassNumber);
					closeNodeAttributes(file, "\n");
					
					
					for(t=0; t<texturesPassNumber; t++)
					{
						MTexturePass * texturePass = material->getTexturePass(t);
						
						MTexture * texture = texturePass->getTexture();
						unsigned int mapChannel = texturePass->getMapChannel();
						M_TEX_COMBINE_MODES combineMode = texturePass->getCombineMode();
						
						
						openAttributeNode(file, "texturePass", 3);
						writeInt(file, "id", t, " ");
						
						if(texture)
						{
							int textureId = getTextureId(mesh, texture);
							writeInt(file, "texture", textureId, " ");
						
							switch(combineMode)
							{
								default:
									writeString(file, "mode", "modulate", " ");
									break;
								case M_TEX_COMBINE_REPLACE:
									writeString(file, "mode", "replace", " ");
									break;
								case M_TEX_COMBINE_ALPHA:
									writeString(file, "mode", "alpha", " ");
									break;
								case M_TEX_COMBINE_DOT:
									writeString(file, "mode", "dot", " ");
									break;
								case M_TEX_COMBINE_ADD:
									writeString(file, "mode", "add", " ");
									break;
								case M_TEX_COMBINE_SUB:
									writeString(file, "mode", "sub", " ");
									break;
							}
							
							writeInt(file, "mapChannel", mapChannel);
						}
						
						closeAttributeNode(file, "\n");
					}
					
					
					closeNode(file, "TexturesPass", 2, "\n");
				}
				
				
				closeNode(file, "Material", 1, "\n");
			}
			
			// close Materials node
			closeNode(file, "Materials", 0, "\n\n");
		}
	}
	
	
	// Bones
	{
		MArmature * armature = mesh->getArmature();
		if(armature)
		{
			unsigned int b, bonesNumber = armature->getBonesNumber();
			if(bonesNumber > 0)
			{
				// open Materials node
				openAttributeNode(file, "Bones", 0);
				writeInt(file, "num", bonesNumber);
				closeNodeAttributes(file, "\n");
				
			
				for(b=0; b<bonesNumber; b++)
				{
					MOBone * bone = armature->getBone(b);
					MObject3d * parent = bone->getParent();
					
					MVector3 position = bone->getPosition();
					MVector3 scale = bone->getScale();
					MVector3 rotation = bone->getRotation().getEulerAngles();
					
					
					openAttributeNode(file, "Bone", 1);
					writeInt(file, "id", b, " ");
					writeString(file, "name", bone->getName());
					closeNodeAttributes(file, "\n");
					
					if(parent)
					{
						unsigned int id;
						if(armature->getBoneId(parent->getName(), &id))
						{
							openAttributeNode(file, "parent", 2);
							writeInt(file, "id", id);
							closeAttributeNode(file, "\n");
						}
					}
					
					openAttributeNode(file, "position", 2);
					writeFloat(file, "x", position.x, " ");
					writeFloat(file, "y", position.y, " ");
					writeFloat(file, "z", position.z);
					closeAttributeNode(file, "\n");
					
					openAttributeNode(file, "rotation", 2);
					writeFloat(file, "x", rotation.x, " ");
					writeFloat(file, "y", rotation.y, " ");
					writeFloat(file, "z", rotation.z);
					closeAttributeNode(file, "\n");
					
					openAttributeNode(file, "scale", 2);
					writeFloat(file, "x", scale.x, " ");
					writeFloat(file, "y", scale.y, " ");
					writeFloat(file, "z", scale.z);
					closeAttributeNode(file, "\n");
					
					closeNode(file, "Bone", 1, "\n");
				}
				
				
				// close Materials node
				closeNode(file, "Bones", 0, "\n\n");
			}
		}
	}
	
	
	// BoundingBox
	{
		MBox3d * box = mesh->getBoundingBox();
		
		openAttributeNode(file, "BoundingBox", 0);
		writeFloat(file, "minx", box->min.x, " ");
		writeFloat(file, "miny", box->min.y, " ");
		writeFloat(file, "minz", box->min.z, " ");
		writeFloat(file, "maxx", box->max.x, " ");
		writeFloat(file, "maxy", box->max.y, " ");
		writeFloat(file, "maxz", box->max.z);
		closeAttributeNode(file, "\n\n");
	}
	
	
	// SubMeshs
	if(mesh->getSubMeshsNumber() > 0)
	{
		unsigned int s, subMeshsNumber = mesh->getSubMeshsNumber();
		MSubMesh * subMeshs = mesh->getSubMeshs();
		
		
		// open SubMeshs node
		openAttributeNode(file, "SubMeshs", 0);
		writeInt(file, "num", subMeshsNumber);
		closeNodeAttributes(file, "\n");
		
		
		for(s=0; s<subMeshsNumber; s++)
		{
			MSubMesh * subMesh = &(subMeshs[s]);

			unsigned int i;
			unsigned int indicesSize = subMesh->getIndicesSize();
			unsigned int verticesSize = subMesh->getVerticesSize();
			unsigned int normalsSize = subMesh->getNormalsSize();
			unsigned int tangentsSize = subMesh->getTangentsSize();
			unsigned int texCoordsSize = subMesh->getTexCoordsSize();
			unsigned int colorsSize = subMesh->getColorsSize();
			
			M_TYPES indicesType = subMesh->getIndicesType();
			void * indices = subMesh->getIndices();
			
			MColor * colors = subMesh->getColors();
			MVector3 * vertices = subMesh->getVertices();
			MVector3 * normals = subMesh->getNormals();
			MVector3 * tangents = subMesh->getTangents();
			MVector2 * texCoords = subMesh->getTexCoords();
			
			MBox3d * box = subMesh->getBoundingBox();
			MSkinData * skin = subMesh->getSkinData();
			map<unsigned int, unsigned int> * mapChannelOffsets = subMesh->getMapChannelOffsets();
			
			
			openAttributeNode(file, "SubMesh", 1);
			writeInt(file, "id", s);
			closeNodeAttributes(file, "\n");
			
			
			// BoundingBox
			openAttributeNode(file, "BoundingBox", 2);
			writeFloat(file, "minx", box->min.x, " ");
			writeFloat(file, "miny", box->min.y, " ");
			writeFloat(file, "minz", box->min.z, " ");
			writeFloat(file, "maxx", box->max.x, " ");
			writeFloat(file, "maxy", box->max.y, " ");
			writeFloat(file, "maxz", box->max.z);
			closeAttributeNode(file, "\n");
			
			
			// vertices
			if(vertices)
			{
				openAttributeNode(file, "Vertices", 2);
				writeInt(file, "num", verticesSize);
				closeNodeAttributes(file, "\n");
				for(i=0; i<verticesSize; i++)
				{
					openAttributeNode(file, "vertex", 3);
					writeFloat(file, "x", vertices[i].x, " ");
					writeFloat(file, "y", vertices[i].y, " ");
					writeFloat(file, "z", vertices[i].z);
					closeAttributeNode(file, "\n");
				}
				closeNode(file, "Vertices", 2, "\n");
			}
			
			// normals
			if(normals)
			{
				openAttributeNode(file, "Normals", 2);
				writeInt(file, "num", normalsSize);
				closeNodeAttributes(file, "\n");
				for(i=0; i<normalsSize; i++)
				{
					openAttributeNode(file, "normal", 3);
					writeFloat(file, "x", normals[i].x, " ");
					writeFloat(file, "y", normals[i].y, " ");
					writeFloat(file, "z", normals[i].z);
					closeAttributeNode(file, "\n");
				}
				closeNode(file, "Normals", 2, "\n");
			}
			
			/*
			// tangents
			if(tangents)
			{
				openAttributeNode(file, "Tangents", 2);
				writeInt(file, "num", tangentsSize);
				closeNodeAttributes(file, "\n");
				for(i=0; i<tangentsSize; i++)
				{
					openAttributeNode(file, "tangent", 3);
					writeFloat(file, "x", tangents[i].x, " ");
					writeFloat(file, "y", tangents[i].y, " ");
					writeFloat(file, "z", tangents[i].z);
					closeAttributeNode(file, "\n");
				}
				closeNode(file, "Tangents", 2, "\n");
			}*/
			
			// texCoords
			if(texCoords)
			{
				int nb_channels = 0;
				char str_channels[256] = "";
				
				map<unsigned int, unsigned int>::iterator
				mit (mapChannelOffsets->begin()),
				mend(mapChannelOffsets->end());
				
				for(;mit!=mend;++mit)
				{
					if(nb_channels == 0)
						sprintf(str_channels, "%d", mit->first);
					else
						sprintf(str_channels, "%s %d", str_channels, mit->first);
					nb_channels++;
				}
				
				openAttributeNode(file, "TexCoords", 2);
				writeInt(file, "num", texCoordsSize, " ");
				writeString(file, "mapChannels", str_channels);
				closeNodeAttributes(file, "\n");
				for(i=0; i<texCoordsSize; i++)
				{
					openAttributeNode(file, "texCoord", 3);
					writeFloat(file, "x", texCoords[i].x, " ");
					writeFloat(file, "y", texCoords[i].y);
					closeAttributeNode(file, "\n");
				}
				closeNode(file, "TexCoords", 2, "\n");
			}
			
			// colors
			if(colors)
			{
				openAttributeNode(file, "Colors", 2);
				writeInt(file, "num", colorsSize);
				closeNodeAttributes(file, "\n");
				for(i=0; i<colorsSize; i++)
				{
					openAttributeNode(file, "color", 3);
					writeFloat(file, "x", colors[i].r/255.0f, " ");
					writeFloat(file, "y", colors[i].g/255.0f, " ");
					writeFloat(file, "z", colors[i].b/255.0f, " ");
					writeFloat(file, "w", colors[i].a/255.0f);
					closeAttributeNode(file, "\n");
				}
				closeNode(file, "Colors", 2, "\n");
			}
			
			// indices
			if(indices)
			{
				openAttributeNode(file, "Indices", 2);
				writeInt(file, "num", indicesSize);
				closeNodeAttributes(file, "\n");
				
				switch(indicesType)
				{
					case M_USHORT:
					{
						for(i=0; i<indicesSize; i++)
						{
							openAttributeNode(file, "index", 3);
							writeInt(file, "value", ((unsigned short*)indices)[i]);
							closeAttributeNode(file, "\n");
						}
						break;
					}
					case M_UINT:
					{
						for(i=0; i<indicesSize; i++)
						{
							openAttributeNode(file, "index", 3);
							writeInt(file, "value", ((unsigned int*)indices)[i]);
							closeAttributeNode(file, "\n");
						}
						break;
					}
				}
				
				closeNode(file, "Indices", 2, "\n");
			}
			
			
			// Skins
			if(skin)
			{
				// skin point
				unsigned int p, pointsNumber = skin->getPointsNumber();
				if(pointsNumber > 0)
				{
					openAttributeNode(file, "Skins", 2);
					writeInt(file, "num", pointsNumber);
					closeNodeAttributes(file, "\n");
				
				
					for(p=0; p<pointsNumber; p++)
					{
						MSkinPoint * skinPoint = skin->getPoint(p);
					
						unsigned int vertexId = skinPoint->getVertexId();
						unsigned int bonesNumber = skinPoint->getBonesNumber();
						unsigned short * bonesIds = skinPoint->getBonesIds();
						float * bonesWeights = skinPoint->getBonesWeights();
					
						openAttributeNode(file, "skin", 3);
						writeInt(file, "vertex", vertexId, " ");
						writeInt(file, "numBones", bonesNumber);
						closeNodeAttributes(file, "\n");
						
						for(i=0; i<bonesNumber; i++)
						{
							openAttributeNode(file, "bone", 4);
							writeInt(file, "id", bonesIds[i], " ");
							writeFloat(file, "weight", bonesWeights[i]);
							closeAttributeNode(file, "\n");
						}
						
						closeNode(file, "skin", 3, "\n");
					}
					
					
					closeNode(file, "Skins", 2, "\n");
				}
			}
			
			
			// Displays
			if(subMesh->getDisplaysNumber() > 0)
			{
				unsigned int d, displaysNumber = subMesh->getDisplaysNumber();
				
				openAttributeNode(file, "Displays", 2);
				writeInt(file, "num", displaysNumber);
				closeNodeAttributes(file, "\n");
					
				
				for(d=0; d<displaysNumber; d++)
				{
					MDisplay * display = subMesh->getDisplay(d);
				
					M_PRIMITIVE_TYPES primitiveType = display->getPrimitiveType();
					unsigned int begin = display->getBegin();
					unsigned int size = display->getSize();
					MMaterial * material = display->getMaterial();
					M_CULL_MODES cullMode = display->getCullMode();
				
					int materialId = getMaterialId(mesh, material);
				
					openAttributeNode(file, "display", 3);
					
					writeString(file, "primType", "triangles", " ");
					writeInt(file, "begin", begin, " ");
					writeInt(file, "size", size, " ");
					writeInt(file, "material", materialId, " ");
					
					switch(cullMode)
					{
						default:
							writeInt(file, "cullFace", 0);
							break;
						case M_CULL_FRONT:
							writeInt(file, "cullFace", 1);
							break;
						case M_CULL_NONE:
							writeInt(file, "cullFace", 2);
							break;
					}
					
					closeAttributeNode(file, "\n");
				}
				
				
				closeNode(file, "Displays", 2, "\n");
			}
			
			
			closeNode(file, "SubMesh", 1, "\n");
		}
		
		
		// close SubMeshs node
		closeNode(file, "SubMeshs", 0, "\n\n");
	}
	
	
	// close Mesh node
	closeNode(file, "Mesh", 0, "\n\n");
	
	// close Maratis node
	closeNode(file, "Maratis", 0);
	

	M_fclose(file);
	return true;
}


// Armature anim export
bool xmlArmatureAnimSave(const char * filename, MArmatureAnim * anim)
{
	// create file
	MFile * file = M_fopen(filename, "wt");
	if(! file)
	{
		MLOG_ERROR("can't create file : " << filename);
		return false;
	}
	
	
	// bones
	unsigned int b, bonesAnimNumber = anim->getBonesAnimNumber();
	MObject3dAnim * bonesAnim = anim->getBonesAnim();
	
	
	// Maratis node
	openNode(file, "Maratis version=\"3.0\"", 0, "\n\n");
	
	// ArmatureAnim node
	openAttributeNode(file, "ArmatureAnim", 0);
	writeInt(file, "num", bonesAnimNumber);
	closeNodeAttributes(file, "\n\n");
	
	
	for(b=0; b<bonesAnimNumber; b++)
	{
		openAttributeNode(file, "Bone", 1);
		writeInt(file, "id", b);
		closeNodeAttributes(file, "\n");
	
	
		MObject3dAnim * objAnim = &(bonesAnim[b]);
	
		unsigned int positionKeysNumber = objAnim->getPositionKeysNumber();
		unsigned int scaleKeysNumber = objAnim->getScaleKeysNumber();
		unsigned int rotationKeysNumber = objAnim->getRotationKeysNumber();
		
		MKey * positionKeys = objAnim->getPositionKeys();
		MKey * scaleKeys = objAnim->getScaleKeys();
		MKey * rotationKeys = objAnim->getRotationKeys();
	
		writeKeys(file, "position", positionKeys, M_VARIABLE_VEC3, positionKeysNumber);
		writeKeys(file, "rotation", rotationKeys, M_VARIABLE_QUAT, rotationKeysNumber);
		writeKeys(file, "scale", scaleKeys, M_VARIABLE_VEC3, scaleKeysNumber);
		
		
		closeNode(file, "Bone", 1, "\n");
	}
	
	
	// close ArmatureAnim node
	closeNode(file, "ArmatureAnim", 0, "\n\n");
	
	// close Maratis node
	closeNode(file, "Maratis", 0);
	
	
	M_fclose(file);
	return true;
}


// Textures anim export
bool xmlTexturesAnimSave(const char * filename, MTexturesAnim * anim)
{
	// create file
	MFile * file = M_fopen(filename, "wt");
	if(! file)
	{
		MLOG_ERROR("can't create file : " << filename);
		return false;
	}
	
	
	// textures
	unsigned int t, texturesAnimNumber = anim->getTexturesAnimNumber();
	MTextureAnim * texturesAnim = anim->getTexturesAnim();
	
	
	// Maratis node
	openNode(file, "Maratis version=\"3.0\"", 0, "\n\n");
	
	// TextureAnim node
	openAttributeNode(file, "TextureAnim", 0);
	writeInt(file, "num", texturesAnimNumber);
	closeNodeAttributes(file, "\n\n");
	
	
	for(t=0; t<texturesAnimNumber; t++)
	{
		openAttributeNode(file, "Texture", 1);
		writeInt(file, "id", t);
		closeNodeAttributes(file, "\n");
	
	
		MTextureAnim * texAnim = &(texturesAnim[t]);
		
		unsigned int translateKeysNumber = texAnim->getTranslateKeysNumber();
		unsigned int scaleKeysNumber = texAnim->getScaleKeysNumber();
		unsigned int rotationKeysNumber = texAnim->getRotationKeysNumber();
		
		MKey * translateKeys = texAnim->getTranslateKeys();
		MKey * scaleKeys = texAnim->getScaleKeys();
		MKey * rotationKeys = texAnim->getRotationKeys();
		
		writeKeys(file, "translate", translateKeys, M_VARIABLE_VEC2, translateKeysNumber);
		writeKeys(file, "scale", scaleKeys, M_VARIABLE_VEC2, scaleKeysNumber);
		writeKeys(file, "rotate", rotationKeys, M_VARIABLE_FLOAT, rotationKeysNumber);
		
		
		closeNode(file, "Texture", 1, "\n");
	}
	
	
	// close TextureAnim node
	closeNode(file, "TextureAnim", 0, "\n\n");
	
	// close Maratis node
	closeNode(file, "Maratis", 0);
	
	
	M_fclose(file);
	return true;
}


// Materials anim export
bool xmlMaterialsAnimSave(const char * filename, MMaterialsAnim * anim)
{
	// create file
	MFile * file = M_fopen(filename, "wt");
	if(! file)
	{
		MLOG_ERROR("can't create file : " << filename);
		return false;
	}
	
	
	// materials
	unsigned int m, materialsAnimNumber = anim->getMaterialsAnimNumber();
	MMaterialAnim * materialsAnim = anim->getMaterialsAnim();
	
	
	// Maratis node
	openNode(file, "Maratis version=\"3.0\"", 0, "\n\n");
	
	// MaterialAnim node
	openAttributeNode(file, "MaterialAnim", 0);
	writeInt(file, "num", materialsAnimNumber);
	closeNodeAttributes(file, "\n\n");
	
	
	for(m=0; m<materialsAnimNumber; m++)
	{
		openAttributeNode(file, "Material", 1);
		writeInt(file, "id", m);
		closeNodeAttributes(file, "\n");
	
	
		MMaterialAnim * matAnim = &(materialsAnim[m]);
		
		unsigned int opacityKeysNumber = matAnim->getOpacityKeysNumber();
		unsigned int shininessKeysNumber = matAnim->getShininessKeysNumber();
		unsigned int customValueKeysNumber = matAnim->getCustomValueKeysNumber();
		unsigned int diffuseKeysNumber = matAnim->getDiffuseKeysNumber();
		unsigned int specularKeysNumber = matAnim->getSpecularKeysNumber();
		unsigned int emitKeysNumber = matAnim->getEmitKeysNumber();
		unsigned int customColorKeysNumber = matAnim->getCustomColorKeysNumber();
		
		MKey * opacityKeys = matAnim->getOpacityKeys();
		MKey * shininessKeys = matAnim->getShininessKeys();
		MKey * customValueKeys = matAnim->getCustomValueKeys();
		MKey * diffuseKeys = matAnim->getDiffuseKeys();
		MKey * specularKeys = matAnim->getSpecularKeys();
		MKey * emitKeys = matAnim->getEmitKeys();
		MKey * customColorKeys = matAnim->getCustomColorKeys();
		
		writeKeys(file, "opacity", opacityKeys, M_VARIABLE_FLOAT, opacityKeysNumber);
		writeKeys(file, "shininess", shininessKeys, M_VARIABLE_FLOAT, shininessKeysNumber);
		writeKeys(file, "customValue", customValueKeys, M_VARIABLE_FLOAT, customValueKeysNumber);
		writeKeys(file, "diffuseColor", diffuseKeys, M_VARIABLE_VEC3, diffuseKeysNumber);
		writeKeys(file, "specularColor", specularKeys, M_VARIABLE_VEC3, specularKeysNumber);
		writeKeys(file, "emitColor", emitKeys, M_VARIABLE_VEC3, emitKeysNumber);
		writeKeys(file, "customColor", customColorKeys, M_VARIABLE_VEC3, customColorKeysNumber);
		
		
		closeNode(file, "Material", 1, "\n");
	}
	
	
	// close MaterialAnim node
	closeNode(file, "MaterialAnim", 0, "\n\n");
	
	// close Maratis node
	closeNode(file, "Maratis", 0);
	
	
	M_fclose(file);
	return true;
}
