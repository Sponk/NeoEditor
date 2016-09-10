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


#ifndef __GLES_RENDERER_H
#define __GLES_RENDERER_H

namespace Neo
{

class GLESRenderer : public Renderer
{
	std::string* m_numVisible;
	unsigned int m_colorShader;
	unsigned int m_texturedShader;
	unsigned int m_objectShader;
	unsigned int m_textShader;

	unsigned int m_mvpMatrixUniform, m_normalMatrixUniform, m_textureModeUniform;
	unsigned int m_opacityUniform, m_diffuseUniform, m_lightsCountUniform, m_vertexLightsCountUniform;
	unsigned int m_modelViewMatrixUniform, m_specularUniform, m_shininessUniform;
	unsigned int m_emitUniform, m_ambientLightUniform;
	
	// Color quad uniforms
	unsigned int m_colorMvpMatrixUniform, m_colorColorUniform;
	
	// Textured quad uniforms
	unsigned int m_textureMvpMatrixUniform, m_textureTextureUniform;
	
	unsigned int m_vertexVbo, m_texcoordVbo;

	static const unsigned int PER_PIXEL_LIGHTS = 4;
	static const unsigned int PER_VERTEX_LIGHTS = 4;

	OLight* m_activePixelLights[PER_PIXEL_LIGHTS];
	OLight* m_activeVertexLights[PER_VERTEX_LIGHTS];
	int m_numPixelLights;
	int m_numVertexLights;

	Matrix4x4 m_matrix;

	unsigned int createShader(const char* vertexSource, const char* fragmentSource);

	void prepareMaterialDisplay(unsigned int fx, Neo::SubMesh* mesh, Neo::MaterialDisplay* display);
	void initVBO(Neo::SubMesh* subMesh);
	int findLights(Neo::Scene* scene, Neo::OEntity* entity, Neo::SubMesh* submesh);
	void sendLight(unsigned int fx, const char* prefix, OLight* l, int num, const Matrix4x4& matrix);

	/**
	 * @brief Draws a completely prepared mesh with the given material display.
	 * @param submesh The submesh to draw
	 * @param display The material display to use
	 * @param primitives Used to override the primitive type dictated by the display. (For wireframes)
	 */
	void drawSubmeshDisplay(SubMesh* submesh, MaterialDisplay* display, PRIMITIVE_TYPES primitives);

public:

	void initialize();
	void destroy();
	static Renderer* getNew(void) { return new GLESRenderer(); }

	static const char * getStaticName(void){ return "GLESRenderer"; }
	const char * getName(void){ return getStaticName(); }
	
	void drawScene(Scene* scene, OCamera* camera);

	// extra
	void drawText(OText * textObj, OCamera* camera);
	void drawText(OText * textObj) { drawText(textObj, NULL);  }
	void drawText2D(OText* text, float x, float y, float rotation);

	void drawEntity(OEntity* e, OCamera* camera);
	void drawEntity(OEntity* e, Scene* scene, OCamera* camera);

	void set2D(const Vector2& size);
	void set2D(unsigned int w, unsigned int h);

	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale, const Vector2& flip, const Vector4& texcoords);

	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale, const Vector2& flip);

	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale);

	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture, float rotation);
	
	void drawColoredQuad(const Vector2& position, const Vector2& size, const Vector4& color, float rotation);

	virtual void sendTexture(unsigned int id, Image* image, bool mipMap, bool filter, bool compress) override;
	virtual unsigned int createTexture() override;
	virtual void destroyTexture(unsigned int id) override;

	unsigned int loadShader(const char* vertPath, const char* fragPath);
	virtual void clearScreen(const Vector3& color) override;
};
}
#endif
