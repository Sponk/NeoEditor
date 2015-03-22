/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MMesh.cpp
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


#include "../Includes/NeoEngine.h"

using namespace Neo;

// MDisplay
MaterialDisplay::MaterialDisplay(PRIMITIVE_TYPES primitiveType, unsigned int begin, unsigned int size):
m_primitiveType(primitiveType),
m_begin(begin),
m_size(size),
m_cullMode(CULL_BACK),
m_material(NULL),
m_visibility(true)
{}

MaterialDisplay::MaterialDisplay(const MaterialDisplay & display):
m_primitiveType(display.m_primitiveType),
m_begin(display.m_begin),
m_size(display.m_size),
m_cullMode(display.m_cullMode),
m_material(display.m_material),
m_visibility(display.m_visibility)
{}

MaterialDisplay::~MaterialDisplay(void)
{}


// MSubMesh
SubMesh::SubMesh(void):

m_morphingData(NULL),
m_skinData(NULL),

m_indicesSize(0),
m_verticesSize(0),
m_normalsSize(0),
m_tangentsSize(0),
m_texCoordsSize(0),
m_colorsSize(0),

m_indicesType(VAR_UINT),
m_indices(NULL),

m_vertices(NULL),
m_normals(NULL),
m_tangents(NULL),
m_texCoords(NULL),
m_colors(NULL),

m_vboId1(0),
m_vboId2(0),

m_displaysNumber(0),
m_displays(NULL)
{}

SubMesh::~SubMesh(void)
{
	clearVBO();
	
	clearIndices();
	clearVertices();
	clearNormals();
	clearTangents();
	clearTexCoords();
	clearColors();

	clearDisplays();

	clearMorphingData();
	clearSkinData();
}

void SubMesh::clearMapChannelOffset(void)
{
	m_mapChannelsOffset.clear();
}

void SubMesh::setMapChannelOffset(unsigned int channel, unsigned int offset)
{
	m_mapChannelsOffset[channel] = offset;
}

bool SubMesh::isMapChannelExist(unsigned int channel)
{
	map<unsigned int, unsigned int>::iterator iter = m_mapChannelsOffset.find(channel);
	return (iter != m_mapChannelsOffset.end());
}

unsigned int SubMesh::getMapChannelOffset(unsigned int channel)
{
	map<unsigned int, unsigned int>::iterator iter = m_mapChannelsOffset.find(channel);
	if(iter != m_mapChannelsOffset.end())
		return iter->second;
	return 0;
}

void SubMesh::clearMorphingData(void)
{
	SAFE_DELETE(m_morphingData);
}

MorphingData * SubMesh::createMorphingData(void)
{
	MorphingData * morphingData = new MorphingData();
	clearMorphingData();
	m_morphingData = morphingData;
	return morphingData;
}

void SubMesh::clearSkinData(void)
{
	SAFE_DELETE(m_skinData);
}

SkinData * SubMesh::createSkinData(void)
{
	SkinData * skinData = new SkinData();
	clearSkinData();
	m_skinData = skinData;
	return skinData;
}

void SubMesh::clearIndices(void)
{
	m_indicesSize = 0;
	SAFE_FREE(m_indices);
}

void SubMesh::clearVertices(void)
{
	m_verticesSize = 0;
	SAFE_DELETE_ARRAY(m_vertices);
}

void SubMesh::clearNormals(void)
{
	m_normalsSize = 0;
	SAFE_DELETE_ARRAY(m_normals);
}

void SubMesh::clearTangents(void)
{
	m_tangentsSize = 0;
	SAFE_DELETE_ARRAY(m_tangents);
}

void SubMesh::clearTexCoords(void)
{
	m_texCoordsSize = 0;
	SAFE_DELETE_ARRAY(m_texCoords);
}

void SubMesh::clearColors(void)
{
	m_colorsSize = 0;
	SAFE_DELETE_ARRAY(m_colors);
}

void * SubMesh::allocIndices(unsigned int size, VAR_TYPES type)
{
	clearIndices();
	if(size == 0)
		return NULL;

	m_indicesSize = size;

	switch(type)
	{
	default:
	case VAR_UINT:
		m_indicesType = VAR_UINT;
		m_indices = malloc(sizeof(unsigned int)*size);
		break;
	case VAR_USHORT:
		m_indicesType = VAR_USHORT;
		m_indices = malloc(sizeof(unsigned short)*size);
		break;
	}
	
	return m_indices;
}

Vector3 * SubMesh::allocVertices(unsigned int size)
{
	clearVertices();
	if(size == 0)
		return NULL;

	m_verticesSize = size;
	m_vertices = new Vector3[size];

	return m_vertices;
}

Vector3 * SubMesh::allocNormals(unsigned int size)
{
	clearNormals();
	if(size == 0)
		return NULL;

	m_normalsSize = size;
	m_normals = new Vector3[size];

	return m_normals;
}

Vector3 * SubMesh::allocTangents(unsigned int size)
{
	clearTangents();
	if(size == 0)
		return NULL;
	
	m_tangentsSize = size;
	m_tangents = new Vector3[size];
	
	return m_tangents;
}

Vector2 * SubMesh::allocTexCoords(unsigned int size)
{
	clearTexCoords();
	if(size == 0)
		return NULL;

	m_texCoordsSize = size;
	m_texCoords = new Vector2[size];

	return m_texCoords;
}

Color * SubMesh::allocColors(unsigned int size)
{
	clearColors();
	if(size == 0)
		return NULL;

	m_colorsSize = size;
	m_colors = new Color[size];

	return m_colors;
}

void SubMesh::clearVBO(void)
{
	NeoEngine * engine = NeoEngine::getInstance();
	RenderingContext * render = engine->getRenderingContext();
	if(render && m_vboId1>0)
	{
		render->deleteVBO(&m_vboId1);
		render->deleteVBO(&m_vboId2);
	}
}

void SubMesh::allocDisplays(unsigned int size)
{
	clearDisplays();
	if(size == 0)
		return;
	m_displays = (MaterialDisplay **) malloc(sizeof(void *)*size);
}

void SubMesh::clearDisplays(void)
{
	unsigned int i;
	for(i=0; i<m_displaysNumber; i++)
		SAFE_DELETE(m_displays[i]);

	m_displaysNumber = 0;
	SAFE_FREE(m_displays);
}

MaterialDisplay * SubMesh::addNewDisplay(PRIMITIVE_TYPES primitiveType, unsigned int begin, unsigned int size)
{
	MaterialDisplay * display = new MaterialDisplay(primitiveType, begin, size);
	m_displays[m_displaysNumber] = display;
	m_displaysNumber++;
	return display;
}

bool SubMesh::hasTransparency(void)
{
	unsigned int i;
	unsigned int displayNumber = getDisplaysNumber();
	for(i=0; i<displayNumber; i++)
	{
		MaterialDisplay * display = getDisplay(i);

		if((! display->isVisible()) || (! display->getMaterial()))
			continue;

		if(display->getMaterial()->getBlendMode() != BLENDING_NONE)
			return true;
	}

	return false;
}


// MMesh
Mesh::Mesh(void):

m_animsRangesNumber(0),
m_animsRanges(NULL),

m_armatureAnimRef(NULL),
m_texturesAnimRef(NULL),
m_materialsAnimRef(NULL),

m_armature(NULL),

m_texturesNumber(0),
m_textures(NULL),

m_materialsNumber(0),
m_materials(NULL),

m_subMeshsNumber(0),
m_subMeshs(NULL)
{}

Mesh::~Mesh(void)
{
	clear();
}

Mesh * Mesh::getNew(void)
{
	return new Mesh();
}

void Mesh::destroy(void)
{
	delete this;
}

void Mesh::clear(void)
{
	clearArmature();
	clearTextures();
	clearMaterials();
	clearSubMeshs();
	clearAnimsRanges();
	
	m_armatureAnimRef = NULL;
	m_texturesAnimRef = NULL;
	m_materialsAnimRef = NULL;
}

void Mesh::clearAnimsRanges(void)
{
	m_animsRangesNumber = 0;
	SAFE_DELETE_ARRAY(m_animsRanges);
}

AnimRange * Mesh::allocAnimsRanges(unsigned int size)
{
	clearAnimsRanges();
	if(size == 0)
		return NULL;

	m_animsRangesNumber = size;
	m_animsRanges = new AnimRange[m_animsRangesNumber];
	return m_animsRanges;
}

ArmatureAnim * Mesh::getArmatureAnim(void)
{
	if(! m_armatureAnimRef)
		return NULL;

	return m_armatureAnimRef->getArmatureAnim();
}

TexturesAnim * Mesh::getTexturesAnim(void)
{
	if(! m_texturesAnimRef)
		return NULL;

	return m_texturesAnimRef->getTexturesAnim();
}

MaterialsAnim * Mesh::getMaterialsAnim(void)
{
	if(! m_materialsAnimRef)
		return NULL;

	return m_materialsAnimRef->getMaterialsAnim();
}

void Mesh::clearArmature(void)
{
	SAFE_DELETE(m_armature);
}

Armature * Mesh::createArmature(void)
{
	clearArmature();
	m_armature = new Armature();
	return m_armature;
}

void Mesh::allocTextures(unsigned int size)
{
	clearTextures();
	if(size == 0)
		return;
	m_textures = (Texture **) malloc(sizeof(void *)*size);
}

void Mesh::clearTextures(void)
{
	unsigned int i;
	for(i=0; i<m_texturesNumber; i++)
		SAFE_DELETE(m_textures[i]);

	m_texturesNumber = 0;
	SAFE_FREE(m_textures);
}

Texture * Mesh::addNewTexture(TextureRef * texRef)
{
	Texture * texture = new Texture(texRef);
	m_textures[m_texturesNumber] = texture;
	m_texturesNumber++;
	return texture;
}

void Mesh::allocMaterials(unsigned int size)
{
	clearMaterials();
	if(size == 0)
		return;
	m_materials = (Material **) malloc(sizeof(void *)*size);
}

void Mesh::clearMaterials(void)
{
	unsigned int i;
	for(i=0; i<m_materialsNumber; i++)
		SAFE_DELETE(m_materials[i]);

	m_materialsNumber = 0;
	SAFE_FREE(m_materials);
}

Material * Mesh::addNewMaterial(void)
{
	Material * material = new Material();
	m_materials[m_materialsNumber] = material;
	m_materialsNumber++;
	return material;
}

SubMesh * Mesh::allocSubMeshs(unsigned int size)
{
	clearSubMeshs();
	if(size == 0)
		return NULL;

	m_subMeshsNumber = size;
	m_subMeshs = new SubMesh[size];
	return m_subMeshs;
}

void Mesh::clearSubMeshs(void)
{
	m_subMeshsNumber = 0;
	SAFE_DELETE_ARRAY(m_subMeshs);
}

void Mesh::updateBoundingBox(void)
{
	if(m_subMeshsNumber > 0)
	{
		Vector3 * min = &m_boundingBox.min;
		Vector3 * max = &m_boundingBox.max;

		Box3d * box = m_subMeshs[0].getBoundingBox();
		(*min) = box->min;
		(*max) = box->max;

		unsigned int i;
		for(i=1; i<m_subMeshsNumber; i++)
		{
			box = m_subMeshs[i].getBoundingBox();

			Vector3 * subMin = &box->min;
			Vector3 * subMax = &box->max;

			min->x = MIN(min->x, subMin->x);
			min->y = MIN(min->y, subMin->y);
			min->z = MIN(min->z, subMin->z);

			max->x = MAX(max->x, subMax->x);
			max->y = MAX(max->y, subMax->y);
			max->z = MAX(max->z, subMax->z);
		}
	}
}
