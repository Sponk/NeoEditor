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


#ifndef __STANDARD_RENDERER_H
#define __STANDARD_RENDERER_H

#define NUM_SHADERS 3

namespace Neo
{
/**
 * @brief The Deferred Rendering Pipeline
 *
 * This class implements a deferred shading algorithm with support for a virtually infinite amount of
 * lights visible at a time. The actual numbers are of course limited by actual hardware.
 * All shading code is loaded from an external shader file to be fully accessible for
 * customisation so implementing custom shading algorithms and post processing effects is quite easy.
 *
 * Rendering happens in several steps:
 * 1. Render all opaque objects (writes to the G-Buffer, position buffer, normal buffer and data buffer)
 * 2. Render the lit image using the data rendered before
 * 3. Render all transparent objects from back to front to the lit image buffer
 * 4. Render the final image using the buffer that contains the complete lit 
 *    image to the screen while using a post effect filter.
 *
 * @todo Shadow mapping
 */
class StandardRenderer : public Renderer
{
    FXRef* m_shaders[NUM_SHADERS];
    unsigned int m_fx[NUM_SHADERS];

	unsigned int m_texturedFx;
	unsigned int m_colorOnlyFx;
	unsigned int m_textureVao;
	unsigned int m_texcoordVbo;
	unsigned int m_vertexVbo;
	unsigned int m_colorVao;
    
	unsigned int m_framebufferID;
    unsigned int m_finalFramebufferID;

    unsigned int m_gbufferTexID;
    unsigned int m_finalTexID;
    unsigned int m_depthTexID;
    unsigned int m_normalTexID;
    unsigned int m_positionTexID;
    unsigned int m_dataTexID;
	unsigned int m_randomTexID;
	
    unsigned int m_quadVAO;
    unsigned int m_textVAO;

    unsigned int m_quadVBO;
    unsigned int m_quadTexCoordVBO;

    unsigned int m_textTexCoordVBO;
    unsigned int m_textVBO;

public:

	StandardRenderer(void);
	~StandardRenderer(void);

	void initQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo, unsigned int fx);
	void clearQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo);

	void drawMesh(Mesh* mesh, OCamera* camera, Material* materials, bool wireframe);
	void drawSubMesh(SubMesh* mesh, OCamera* camera, Material* materials, bool wireframe);
	void drawDisplay(SubMesh* mesh, MaterialDisplay* display, OCamera* camera, Material* materials, bool wireframe);

	void sendLight(unsigned int fx, OLight* l, int num, Matrix4x4 matrix);

	void drawGBuffer(Scene* scene, OCamera* camera);

	void drawTransparents(Scene* scene, OCamera* camera);
	void initVBO(SubMesh * subMesh);

	void initFramebuffers(Vector2 res = Vector2(0,0));
	void clearFramebuffers();

	void renderFinalImage(Scene *scene, OCamera* camera, bool postFx = false);

	void set2D(unsigned int w, unsigned int h);
	void drawQuad(int fx);

	void setShadingAlgorithm(const char* str);
	
	// All visible lights. Should be updated by a worker thread.
	static int light_update_thread(void* data);
	Thread* m_lightUpdateThread;

	Semaphore* m_lightUpdateSemaphore;
	Semaphore* m_visibilityUpdateSemaphore;
	bool m_threadExit; /// Used to signal the threads to exit

	// Cache texture coordinates for faster rendering
	Vector2 m_texCoords[4];

	// Cache vertices for faster rendering
	Vector2 m_vertices[4];
	Vector2 m_resolution;

	std::string* m_visibleObjects;

private:
	void drawDisplay(SubMesh * subMesh, MaterialDisplay * display, Vector3 * vertices, Vector3 * normals, Vector3 * tangents, Color * colors);
	void drawOpaques(SubMesh * subMesh, Armature * armature);
	void drawTransparents(SubMesh * subMesh, Armature * armature);
	void updateVisibility(Scene * scene, OCamera * camera);
	
	float getDistanceToCam(OCamera * camera, const Vector3 & pos);
	void updateSkinning(Mesh* mesh, Armature* armature);
	void loadShader(const char* vert, const char* frag, unsigned int* fx);
	void init2DVao(unsigned int fx, unsigned int* vao);
	
public:
	
	// destroy
	void destroy(void);
	
	// get new
	static Renderer * getNew(void) { return new StandardRenderer(); }

	// name
	static const char * getStaticName(void){ return "StandardRenderer"; }
	const char * getName(void){ return getStaticName(); }
	
	// draw
	void drawScene(Scene * scene, OCamera * camera);
	void drawToTexture(Scene * scene, OCamera* camera, unsigned int texId);

	// extra
	void drawText(OText * textObj, OCamera* camera);
	void drawText(OText * textObj) { drawText(textObj, NULL);  }
	void drawText2D(OText* text, float x, float y, float rotation);

	void drawEntity(OEntity* e, OCamera* camera);

	void drawDisplay(OEntity* e, SubMesh* mesh, MaterialDisplay* display, OCamera* camera);
	void drawSubmesh(OEntity* e, SubMesh* submesh, OCamera* camera);

	void initialize();
	void smallInit(unsigned int width, unsigned int height);
	void set2D(const Vector2& size);
		
	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale, const Vector2& flip, const Vector4& texcoords);

	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale, const Vector2& flip);

	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
		float rotation, const Vector2& scale);

	void drawTexturedQuad(const Vector2& position, const Vector2& size, int texture, float rotation);
	
	void drawColoredQuad(const Vector2& position, const Vector2& size, const Vector4& color,
		float rotation);

	virtual void sendTexture(unsigned int id, Image* image, bool mipMap, bool filter, bool compress) override;
	virtual unsigned int createTexture() override;
	virtual void destroyTexture(unsigned int id) override;
	
	virtual void enableScissors(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
	virtual void disableScissors();
	
	virtual void enableDepthTest(bool value);
	
private:

	void renderShadows(Scene* scene, OCamera* maincam);
	void prepareSubmesh(SubMesh* mesh);
	void prepareMaterialDisplay(SubMesh* mesh, MaterialDisplay* display);
	
	void sendLights(unsigned int shader, OCamera* camera);
	void initTextVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo, unsigned int fx);
	
	static int visibility_thread_mainscene(void* data);
	Thread* m_visibilityThread;

	void KillThreads();

public:
	virtual void clearScreen(const Vector3& color) override;
};
}
#endif
