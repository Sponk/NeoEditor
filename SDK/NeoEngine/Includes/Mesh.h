//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
// Copyright (c) 2014-2015 Yannick Pflanzer <www.neo-engine.de>
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

#ifndef __MESH_H
#define __MESH_H

#include "MorphingData.h"
#include "SkinData.h"

namespace Neo
{
// Display
class NEO_ENGINE_EXPORT MaterialDisplay
{
public:
	MaterialDisplay(PRIMITIVE_TYPES primitiveType, unsigned int begin,
					unsigned int size);

	MaterialDisplay(const MaterialDisplay &display);
	~MaterialDisplay(void);

private:
	// draw array infos
	PRIMITIVE_TYPES m_primitiveType;
	unsigned int m_begin;
	unsigned int m_size;

	// cull mode
	CULL_MODES m_cullMode;

	// material
	Material *m_material;

	// visibility
	bool m_visibility;

	unsigned int m_vao;

public:
	// array infos
	inline PRIMITIVE_TYPES getPrimitiveType(void) { return m_primitiveType; }
	inline unsigned int getBegin(void) { return m_begin; }
	inline unsigned int getSize(void) { return m_size; }

	// cull mode
	inline void setCullMode(CULL_MODES cullMode) { m_cullMode = cullMode; }
	inline CULL_MODES getCullMode(void) { return m_cullMode; }

	// material
	inline void setMaterial(Material *material) { m_material = material; }
	inline Material *getMaterial(void) { return m_material; }

	// visibility
	inline void setVisibility(bool visibility) { m_visibility = visibility; }
	inline bool isVisible(void) { return m_visibility; }

	inline unsigned int* getVAO() { return &m_vao; }
};

// SubMesh
class NEO_ENGINE_EXPORT SubMesh
{
public:
	SubMesh(void);
	~SubMesh(void);

private:
	// morphing data
	MorphingData *m_morphingData;

	// skin data
	SkinData *m_skinData;

	// data
	unsigned int m_indicesSize;
	unsigned int m_verticesSize;
	unsigned int m_normalsSize;
	unsigned int m_tangentsSize;
	unsigned int m_texCoordsSize;
	unsigned int m_colorsSize;

	VAR_TYPES m_indicesType;
	void* m_indices;

	Color* m_colors;
	Vector3* m_vertices;
	Vector3* m_normals;
	Vector3* m_tangents;
	Vector2* m_texCoords;

	Vector3* m_skinVertices;
	Vector3* m_skinNormals;
	Vector3* m_skinTangents;

	// VBO (optional)
	unsigned int m_vboId1;
	unsigned int m_vboId2; // for indices

	// mapChannels offset (map of textureCoords offsets used for channel
	// mapping)
	map<unsigned int, unsigned int> m_mapChannelsOffset;

	// displays
	unsigned int m_displaysNumber;
	MaterialDisplay **m_displays;

	// bounding box
	Box3d m_boundingBox;

public:
	// morphing data
	void clearMorphingData(void);
	MorphingData *createMorphingData(void);
	inline MorphingData *getMorphingData(void) { return m_morphingData; }

	// skin data
	void clearSkinData(void);
	SkinData *createSkinData(void);
	inline SkinData *getSkinData(void) { return m_skinData; }

	// data
	void clearIndices(void);
	void clearVertices(void);
	void clearNormals(void);
	void clearTangents(void);
	void clearTexCoords(void);
	void clearColors(void);

	void *allocIndices(unsigned int size, VAR_TYPES type);
	Color *allocColors(unsigned int size);
	Vector3 *allocVertices(unsigned int size);
	Vector3 *allocNormals(unsigned int size);
	Vector3 *allocTangents(unsigned int size);
	Vector2 *allocTexCoords(unsigned int size);

	inline unsigned int getIndicesSize(void) { return m_indicesSize; }
	inline unsigned int getVerticesSize(void) { return m_verticesSize; }
	inline unsigned int getNormalsSize(void) { return m_normalsSize; }
	inline unsigned int getTangentsSize(void) { return m_tangentsSize; }
	inline unsigned int getTexCoordsSize(void) { return m_texCoordsSize; }
	inline unsigned int getColorsSize(void) { return m_colorsSize; }

	VAR_TYPES getIndicesType(void) { return m_indicesType; }
	inline void *getIndices(void) { return m_indices; }

	inline Color *getColors(void) { return m_colors; }
	inline Vector3 *getVertices(void) { return m_vertices; }
	inline Vector3 *getNormals(void) { return m_normals; }
	inline Vector3 *getTangents(void) { return m_tangents; }
	inline Vector2 *getTexCoords(void) { return m_texCoords; }

	inline Vector3 *getSkinVertices(void) { return m_skinVertices; }
	inline Vector3 *getSkinNormals(void) { return m_skinNormals; }
	inline Vector3 *getSkinTangents(void) { return m_skinTangents; }


	// VBO
	void clearVBO(void);
	unsigned int *getVBOid1(void) { return &m_vboId1; }
	unsigned int *getVBOid2(void) { return &m_vboId2; }

	// mapChannels offset
	void clearMapChannelOffset(void);
	void setMapChannelOffset(unsigned int channel, unsigned int offset);
	bool isMapChannelExist(unsigned int channel);
	unsigned int getMapChannelOffset(unsigned int channel);
	
	map<unsigned int, unsigned int> *getMapChannelOffsets(void)
	{
		return &m_mapChannelsOffset;
	}

	// displays
	void allocDisplays(unsigned int size);
	void clearDisplays(void);
	MaterialDisplay *addNewDisplay(PRIMITIVE_TYPES primitiveType,
								   unsigned int begin, unsigned int size);
	
	inline unsigned int getDisplaysNumber(void) { return m_displaysNumber; }

	inline MaterialDisplay *getDisplay(unsigned int id)
	{
		return m_displays[id];
	}

	// bounding box
	inline Box3d *getBoundingBox(void) { return &m_boundingBox; }

	// transparency
	bool hasTransparency(void);
};

/**
 * @brief Describes a Range in an animation starting at a certain frame
 * and ending at another certain frame.
 */
class NEO_ENGINE_EXPORT AnimRange : public Range
{
public:
	AnimRange(void) : loops(-1) {}

	/// How should the animation loop
	int loops;
};

/**
 * @brief The Mesh class contains the implementation of a generic
 * (i.e. file format agnostic) 3D mesh with support for armature animation,
 * material animation and multitexturing.
 */
class NEO_ENGINE_EXPORT Mesh
{
public:

	Mesh(void);
	~Mesh(void);

	/**
	 * @brief Creates a new Mesh instance.
	 *
	 * Used by the mesh factory class.
	 *
	 * @return The new Mesh.
	 */
	static Mesh * getNew(void);

	/**
	 * @brief Destroys the mesh instance and frees all allocated memory.
	 */
	void destroy(void);

private:

	// anims ranges
	unsigned int m_animsRangesNumber;
	AnimRange * m_animsRanges;

	// armature anim
	ArmatureAnimRef * m_armatureAnimRef;

	// textures anim
	TexturesAnimRef * m_texturesAnimRef;

	// materials anim
	MaterialsAnimRef * m_materialsAnimRef;

	// armature
	Armature * m_armature;

	// textures
	unsigned int m_texturesNumber;
	Texture ** m_textures;

	// materials
	unsigned int m_materialsNumber;
	Material ** m_materials;

	// subMeshs
	unsigned int m_subMeshsNumber;
	SubMesh * m_subMeshs;

	// bounding box
	Box3d m_boundingBox;

public:

	// clear
	void clear(void);

	// anims ranges
	void clearAnimsRanges(void);
	AnimRange *allocAnimsRanges(unsigned int size);

	/**
	 * @brief Returns the number of animation ranges.
	 * @return The number animation ranges.
	 */
	inline unsigned int getAnimsRangesNumber(void)
	{
		return m_animsRangesNumber;
	}

	/**
	 * @brief Returns the array of animation ranges.
	 * @return The array of animation ranges.
	 */
	inline AnimRange *getAnimsRanges(void) { return m_animsRanges; }

	/**
	 * @brief Sets the armature reference.
	 * @param armatureAnimRef The new armature animation reference.
	 *
	 * @see ArmatureAnimRef
	 */
	inline void setArmatureAnimRef(ArmatureAnimRef *armatureAnimRef)
	{
		m_armatureAnimRef = armatureAnimRef;
	}

	/**
	 * @brief Returns the current armature animation reference.
	 * @return The armature animation reference.
	 * @see ArmatureAnimRef
	 */
	inline ArmatureAnimRef *getArmatureAnimRef(void)
	{
		return m_armatureAnimRef;
	}

	ArmatureAnim *getArmatureAnim(void);

	// textures anim
	inline void setTexturesAnimRef(TexturesAnimRef *texturesAnimRef)
	{
		m_texturesAnimRef = texturesAnimRef;
	}
	inline TexturesAnimRef *getTexturesAnimRef(void)
	{
		return m_texturesAnimRef;
	}

	TexturesAnim *getTexturesAnim(void);

	/**
	 * @brief Sets the material animation reference.
	 * @param armatureAnimRef The new material animation reference.
	 *
	 * @see MaterialsAnimRef
	 */
	inline void setMaterialsAnimRef(MaterialsAnimRef *materialsAnimRef)
	{
		m_materialsAnimRef = materialsAnimRef;
	}

	/**
	 * @brief Returns the current material animation reference.
	 * @return The material animation reference.
	 * @see MaterialsAnimRef
	 */
	inline MaterialsAnimRef *getMaterialsAnimRef(void)
	{
		return m_materialsAnimRef;
	}

	MaterialsAnim *getMaterialsAnim(void);

	// armature
	void clearArmature(void);
	Armature *createArmature(void);
	inline Armature *getArmature(void) { return m_armature; }

	// textures
	void allocTextures(unsigned int size);
	void clearTextures(void);
	Texture *addNewTexture(TextureRef *texRef);
	inline unsigned int getTexturesNumber(void) { return m_texturesNumber; }
	inline Texture *getTexture(unsigned int id) { return m_textures[id]; }

	/**
	 * @brief Allocates enough memory for all materials.
	 * @param size The number of materials to allocate.
	 */
	void allocMaterials(unsigned int size);

	/**
	 * @brief Deletes all contained materials.
	 */
	void clearMaterials(void);

	/**
	 * @brief Creates a new Material, adds it to the internal array and returns
	 * it.
	 * @return The new Material.
	 */
	Material *addNewMaterial(void);

	/**
	 * @brief Returns the number of currently active materials.
	 * @return The number of active materials.
	 */
	inline unsigned int getMaterialsNumber(void) { return m_materialsNumber; }

	/**
	 * @brief Retrieves a specific material from the internal array.
	 * @param id The index to the array.
	 * @return The material
	 */
	inline Material *getMaterial(unsigned int id) { return m_materials[id]; }

	void clearSubMeshs(void);
	SubMesh *allocSubMeshs(unsigned int size);
	inline unsigned int getSubMeshsNumber(void) { return m_subMeshsNumber; }
	inline SubMesh *getSubMeshs(void) { return m_subMeshs; }

	/**
	 * @brief Recalculates the bounding box to contain all vertices
	 * of the mesh.
	 */
	void updateBoundingBox(void);

	/**
	 * @brief Returns the current bounding box.
	 * @return The bounding box.
	 * @see updateBoundingBox
	 */
	inline Box3d *getBoundingBox(void) { return &m_boundingBox; }
};
}
#endif
