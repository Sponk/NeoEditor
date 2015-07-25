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


#ifndef __STANDARD_RENDERER_H
#define __STANDARD_RENDERER_H

#define NUM_SHADERS 4

#define MAX_ENTITY_LIGHTS 256
#define MAX_SHADOW_LIGHTS 64
#define MAX_OPAQUE 4096
#define MAX_TRANSP 2048

namespace Neo
{
// Standard Renderer
class StandardRenderer : public Renderer
{
public:
	
	StandardRenderer(void);
	~StandardRenderer(void);
	
	//bool m_isInitialized;
	FXRef* m_shaders[NUM_SHADERS];
	unsigned int m_fx[NUM_SHADERS];

	unsigned int m_framebufferID;
	unsigned int m_gbufferTexID;
	unsigned int m_depthTexID;
	unsigned int m_normalTexID;
	unsigned int m_positionTexID;
	unsigned int m_dataTexID;

	unsigned int m_quadVAO;
	unsigned int m_textVAO;

	unsigned int m_quadVBO;
	unsigned int m_quadTexCoordVBO;

	unsigned int m_textTexCoordVBO;
	unsigned int m_textVBO;

	void initQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo, unsigned int fx);
	void clearQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo);

	void drawMesh(Mesh* mesh, OCamera* camera);
	void drawSubMesh(SubMesh* mesh, OCamera* camera);
	void drawDisplay(SubMesh* mesh, MaterialDisplay* display, OCamera* camera);

	void sendLight(unsigned int fx, OLight* l, int num, Matrix4x4 matrix);

	void drawGBuffer(Scene* scene, OCamera* camera);

	void drawTransparents(Scene* scene, OCamera* camera);
	void initVBO(SubMesh * subMesh);

	void initFramebuffers(Vector2 res = Vector2(0,0));
	void clearFramebuffers();

	void renderFinalImage(Scene *scene, OCamera* camera);

	void set2D(unsigned int w, unsigned int h);
	void drawQuad(int fx);

	// All visible lights. Should be updated by a worker thread.
	static int light_update_thread(void* data);
	OLight* m_visibleLights[MAX_ENTITY_LIGHTS];
	Thread* m_lightUpdateThread;
	Semaphore* m_lightUpdateSemaphore;
	Scene* m_currentScene;
	int m_numVisibleLights;

	// Cache texture coordinates for faster rendering
	Vector2 m_texCoords[4];

	// Cache vertices for faster rendering
	Vector2 m_vertices[4];
	Vector2 m_resolution;

private:
	
	// lists
	int m_transpSortList[MAX_TRANSP];
	int m_opaqueSortList[MAX_OPAQUE];
	float m_transpSortZList[MAX_TRANSP];
	float m_opaqueSortZList[MAX_OPAQUE];
	
	void drawDisplay(SubMesh * subMesh, MaterialDisplay * display, Vector3 * vertices, Vector3 * normals, Vector3 * tangents, Color * colors);
	//void drawDisplayTriangles(MSubMesh * subMesh, MDisplay * display, MVector3 * vertices);
	void drawOpaques(SubMesh * subMesh, Armature * armature);
	void drawTransparents(SubMesh * subMesh, Armature * armature);
	void updateVisibility(Scene * scene, OCamera * camera);
	
	float getDistanceToCam(OCamera * camera, const Vector3 & pos);
	
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

	void drawEntity(OEntity* e, OCamera* camera);
	void drawDisplay(OEntity* e, SubMesh* mesh, MaterialDisplay* display, OCamera* camera);
	void drawSubmesh(OEntity* e, SubMesh* submesh, OCamera* camera);

	void init();
	void smallInit(unsigned int width, unsigned int height);

private:

	void prepareSubmesh(SubMesh* mesh);
	void prepareMaterialDisplay(SubMesh* mesh, MaterialDisplay* display);
	
	void sendLights(unsigned int shader, OCamera* camera);
	void initTextVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo, unsigned int fx);
	
	static int visibility_thread_mainscene(void* data);
	Thread* m_visibilityThread;
};
}
#endif
