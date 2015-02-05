//========================================================================
// Copyright (c) 2015 Yannick Pflanzer <www.neo-engine.de>
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

#ifndef NEO_COMMON_INCLUDE_DEFERREDRENDERER_H_
#define NEO_COMMON_INCLUDE_DEFERREDRENDERER_H_

#include <NeoEngine.h>

#define DEFERRED_SHADERS 3
#define MAX_ENTITY_LIGHTS 256

namespace Neo
{

class DeferredRenderer: public Renderer
{
	FXRef* m_shaders[DEFERRED_SHADERS];
    unsigned int m_fx[DEFERRED_SHADERS];

    unsigned int m_framebufferID;
    unsigned int m_gbufferTexID;
    unsigned int m_depthTexID;
    unsigned int m_normalTexID;
    unsigned int m_positionTexID;

    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
    unsigned int m_quadTexCoordVBO;

    void initQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo, unsigned int fx);
    void clearQuadVAO(unsigned int* vao, unsigned int *vbo, unsigned int *texcoordVbo);

    void drawMesh(Mesh* mesh, OCamera* camera);
    void drawSubMesh(SubMesh* mesh, OCamera* camera);
    void drawDisplay(SubMesh* mesh, MaterialDisplay* display, OCamera* camera);

    void sendLight(unsigned int fx, OLight* l, int num, MMatrix4x4 matrix);

	void drawGBuffer(Scene* scene, OCamera* camera);
    void initVBO(SubMesh * subMesh);

    void initFramebuffers();
    void clearFramebuffers();

    void renderFinalImage(OCamera* camera);

    void set2D(unsigned int w, unsigned int h);
    void drawQuad(int fx);

    // All visible lights. Should be updated by a worker thread.
    static int light_update_thread(void* data);
    OLight* m_visibleLights[MAX_ENTITY_LIGHTS];
    MThread* m_lightUpdateThread;
    MSemaphore* m_lightUpdateSemaphore;
    Scene* m_currentScene;
    int m_numVisibleLights;

    // Cache texture coordinates for faster rendering
    MVector2 m_texCoords[4];

    // Cache vertices for faster rendering
    MVector2 m_vertices[4];

    MVector2 m_resolution;

public:
	DeferredRenderer();
	~DeferredRenderer();

	virtual void destroy(void);
	virtual const char* getName(void);
	virtual void drawScene(Scene* scene, OCamera* camera);
	virtual void drawText(OText* text);

	// FIXME: Don't register as "StandardRenderer"!!!
	static const char* getStaticName() {return "StandardRenderer"; }
	static Renderer* getNew() { return new DeferredRenderer(); }

	virtual void init();
};

#ifndef USE_LEGACY_GL
	typedef DeferredRenderer StandardRenderer;
#endif

} /* namespace Neo */

#endif /* NEO_COMMON_INCLUDE_DEFERREDRENDERER_H_ */
