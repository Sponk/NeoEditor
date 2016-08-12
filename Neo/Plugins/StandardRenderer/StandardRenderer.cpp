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

#include <NeoEngine.h>
#include "StandardRenderer.h"
#include <string>
#include <algorithm>
#include <tinyutf8.h>
#include <atomic>

#ifndef SHADER_PATH
#define SHADER_PATH "./"
#endif

using namespace Neo;

/*
 * Additional scene data
 */
class SceneData: public Scene::AdditionalData
{
public:
	// FIXME: Check for sub meshes?
	std::vector<OEntity*> visibleEntities;
	Semaphore* visibilityLock;

	SceneData()
	{
		visibilityLock = ThreadFactory::getInstance()->getNewSemaphore();
		visibilityLock->Init(1);
	}

	~SceneData()
	{
		SAFE_DELETE(visibilityLock);
	}
};

/*
 * Additional Object3d data
 */
class CameraData: public Object3d::AdditionalData
{
public:
	// FIXME: Check for sub meshes?
	std::vector<OEntity*> visibleEntities;
	std::vector<OEntity*> visibleTransparentEntities;
	std::vector<OLight*> visibleLights;
	Image lightData;
	unsigned int lightTexture;

	CameraData()
	{
		visibleLights.reserve(256);
		visibleEntities.reserve(256);
		visibleTransparentEntities.reserve(256);

		lightTexture = 0;
	}

	~CameraData()
	{

	}
};

/*
 * Additional Light data
 */
class LightData: public Object3d::AdditionalData
{
public:
	// Visible objects for shadow mapping
	std::vector<OEntity*> visibleEntities;

	uint32_t depthTexture;
	uint32_t fbo;

	float resolution;
	OCamera camera;

	LightData()
		:
		depthTexture(0),
		fbo(0),
		resolution(0)
	{}

	~LightData()
	{

	}
};

/*
 * Additional OEntity and OLight data
 */
class EntityData: public Object3d::AdditionalData
{
	unsigned int m_framebuffer;
	unsigned int m_texture[6];
	OCamera* m_camera[6];
	Object3d* m_parent;
	Scene* m_scene;
	StandardRenderer* m_renderer;

public:

	EntityData()
		:
		m_parent(nullptr),
		m_scene(nullptr)
	{

	}

	void drawCubemap()
	{
		NeoEngine* engine = NeoEngine::getInstance();
		StandardRenderer* renderer = static_cast<StandardRenderer*>(engine->getRenderer());
		RenderingContext* render = engine->getRenderingContext();

		render->bindFrameBuffer(m_framebuffer);
		render->attachFrameBufferTexture(ATTACH_COLOR0, m_texture[0]);

		// Enable them to be drawn
		//FRAME_BUFFER_ATTACHMENT buffers[1] = { ATTACH_COLOR0 };
		//render->setDrawingBuffers(buffers, 1);

		for (int i = 0; i < 6; i++)
			m_renderer->drawToTexture(m_scene, m_camera[i], 0);

		//MLOG_INFO("RENDER");
		render->bindFrameBuffer(0);

		//render->setViewport(0, 0, screenWidth, screenHeight); // change viewport
	}

	void initTextures(int res, Scene* scene, Object3d* parent)
	{
		if (m_scene) return;

		m_renderer = new StandardRenderer();
		RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
		m_parent = parent;
		m_scene = scene;

		m_renderer->smallInit(res, res);

		render->createFrameBuffer(&m_framebuffer);

		// GBuffer texture
		for (int i = 0; i < 6; i++)
		{
			render->createTexture(&m_texture[i]);
			render->bindTexture(m_texture[i]);
			render->setTextureFilterMode(TEX_FILTER_LINEAR, TEX_FILTER_LINEAR);
			render->setTextureUWrapMode(WRAP_CLAMP);
			render->setTextureVWrapMode(WRAP_CLAMP);
			render->texImage(0, res, res, VAR_FLOAT, TEX_RGBA, 0);

			m_camera[i] = scene->addNewCamera();
			m_camera[i]->linkTo(parent);
			m_camera[i]->setFov(90.0f);
		}

		m_camera[0]->rotate(Vector3(1, 0, 0), -90);
		m_camera[1]->rotate(Vector3(0, 1, 0), 90);

		m_camera[3]->rotate(Vector3(1, 0, 0), 90);
		m_camera[4]->rotate(Vector3(0, 1, 0), -90);
		m_camera[5]->rotate(Vector3(0, 1, 0), 180);
	}

	void bindCubemap()
	{

	}
};

static const char* defaultShaderFiles[] = {"Default.vert", "Forward.frag", "Final.frag", "Shadow.frag"};

static const char* subroutines[5] =
	{"cookModelColor", "cookModelDiffuse", "cookModelDiffuseSpecular", "cookModelDiffuseNormal",
	 "cookModelDiffuseNormalSpecular"};
//static const char* subroutines[5] = { "phongModelColor", "phongModelDiffuse", "phongModelDiffuseSpecular", "phongModelDiffuseNormal", "phongModelDiffuseNormalSpecular" };

static const char* shadingAlgorithms[] = {"cookTorranceShading", "phongShading"};

StandardRenderer::StandardRenderer() :
	m_colorOnlyFx(0), 
	m_texturedFx(0), 
	m_colorVao(0), 
	m_textureVao(0)
{
	// Let's hope we have an GL context
	for (int i = 0; i < NUM_SHADERS; i++)
	{
		m_shaders[i] = NULL;
		m_fx[i] = -1;
	}

	m_framebufferID = 0;
	m_finalFramebufferID = 0;
	m_gbufferTexID = 0;
	m_depthTexID = 0;
	m_finalTexID = 0;

	m_quadTexCoordVBO = 0;
	m_quadVAO = 0;
	m_quadVBO = 0;

	m_lightUpdateThread = NULL;
	m_lightUpdateSemaphore = NULL;

	m_texCoords[0] = Vector2(0, 1);
	m_texCoords[1] = Vector2(0, 0);
	m_texCoords[3] = Vector2(1, 0);
	m_texCoords[2] = Vector2(1, 1);

	//init();
}

StandardRenderer::~StandardRenderer()
{

}

void StandardRenderer::setShadingAlgorithm(const char* str)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	render->selectSubroutine(m_fx[1], M_SHADER_PIXEL, str);
}

void StandardRenderer::drawMesh(Mesh* mesh, OCamera* camera, Material* materials, bool wireframe)
{
	int num = mesh->getSubMeshsNumber();
	SubMesh* subMeshes = mesh->getSubMeshs();
	Armature* armature = mesh->getArmature();

	for (int i = 0; i < num; i++)
	{
		SkinData* skinData = subMeshes[i].getSkinData();

		if (armature && skinData)
		{
			unsigned int verticesSize = subMeshes[i].getVerticesSize();
			Vector3* skinVertices = subMeshes[i].getSkinVertices();
			Vector3* skinNormals = subMeshes[i].getSkinNormals();
			Vector3* skinTangents = subMeshes[i].getSkinTangents();

			computeSkinning(armature, skinData, subMeshes[i].getVertices(), subMeshes[i].getNormals(),
							subMeshes[i].getTangents(), skinVertices, skinNormals, skinTangents);

			subMeshes[i].getBoundingBox()->initFromPoints(skinVertices, verticesSize);
		}

		drawSubMesh(&subMeshes[i], camera, materials, wireframe);
	}

	mesh->updateBoundingBox();
}

void StandardRenderer::drawSubMesh(SubMesh* mesh, OCamera* camera, Material* materials, bool wireframe)
{
	NeoEngine* engine = NeoEngine::getInstance();

	initVBO(mesh);

	for (int i = 0; i < mesh->getDisplaysNumber(); i++)
	{
		drawDisplay(mesh, mesh->getDisplay(i), camera, materials, wireframe);
	}
}

void StandardRenderer::drawDisplay(SubMesh* mesh,
								   MaterialDisplay* display,
								   OCamera* camera,
								   Material* materials,
								   bool wireframe)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	// Retrieve VBO information
	unsigned int* vboId1 = mesh->getVBOid1();
	unsigned int* vboId2 = mesh->getVBOid2();
	unsigned int* vao = display->getVAO();

	// Save data into variables
	Vector2* texcoords = mesh->getTexCoords();
	VAR_TYPES indicesType = mesh->getIndicesType();
	void* indices = mesh->getIndices();

	Material* material = &materials[display->getMaterialId()];
	int texturePasses = material->getTexturesPassNumber();

	int fx = 0;
	if (*vao == 0)
	{
		prepareMaterialDisplay(mesh, display);
		return;
	}

	fx = m_fx[0];
	render->bindFX(fx);
	render->bindVAO(*vao);

	for (int i = 0; i < texturePasses; i++)
	{
		TexturePass* pass = material->getTexturePass(i);

		if (!pass || !pass->getTexture())
		{
			continue;
		}

		TextureRef* tex = pass->getTexture()->getTextureRef();
		render->bindTexture(tex->getTextureId(), i);
	}

	// Texture
	int texIds[4] = {0, 1, 2, 3};
	render->sendUniformInt(fx, "Textures", texIds, 4);

	// Send the texture mode
	render->sendUniformInt(fx, "TextureMode", &texturePasses);
	render->selectSubroutine(fx, M_SHADER_PIXEL, subroutines[texturePasses]);

	// Set up modelview matrix
	Matrix4x4 modelViewMatrix;
	Matrix4x4 modelViewProjectionMatrix;
	Matrix4x4 modelMatrix;
	Matrix4x4 normalMatrix;

	render->getModelViewMatrix(&modelMatrix);

	modelViewMatrix = *camera->getCurrentViewMatrix() * modelMatrix;
	modelViewProjectionMatrix = *camera->getCurrentProjMatrix() * modelViewMatrix;
	normalMatrix = modelViewMatrix.getInverse().getTranspose();

	// Send uniforms
	render->sendUniformMatrix(m_fx[0], "ProjModelViewMatrix", &modelViewProjectionMatrix);
	render->sendUniformMatrix(m_fx[0], "ModelViewMatrix", &modelViewMatrix);
	render->sendUniformVec3(m_fx[0], "Diffuse", material->getDiffuse());
	render->sendUniformVec3(m_fx[0], "Specular", material->getSpecular());
	render->sendUniformVec3(m_fx[0], "Emit", material->getEmit());

	int hasTransparency = (mesh->hasTransparency()) ? 1 : 0;
	float opacity = material->getOpacity();

	render->sendUniformInt(m_fx[0], "HasTransparency", &hasTransparency);

	if (hasTransparency)
	{
		render->enableBlending();
		render->setBlendingMode(material->getBlendMode());
		render->disableCullFace();
		render->setDepthMask(false);
	}
	else
	{
		render->setDepthMask(true);
		render->enableCullFace();
		render->disableBlending();
	}

	float shininess = material->getShininess(); // sqrt(2.0f/(1.25f*material->getShininess()));
	render->sendUniformFloat(m_fx[0], "Shininess", &shininess);
	render->sendUniformFloat(m_fx[0], "Opacity", &opacity);

	// Set up normal matrix
	render->sendUniformMatrix(m_fx[0], "NormalMatrix", &normalMatrix);

	// Set cull mode
	if (display->getCullMode() != CULL_NONE)
		render->setCullMode(display->getCullMode());
	else
		render->disableCullFace();

	render->enableDepthTest();

	if (indices) // If the SubMesh has indices
	{
		if (*vboId2 > 0) // If the indices are stored in the VBO
		{
			render->bindVBO(VBO_ELEMENT_ARRAY, *vboId2);
			switch (indicesType)
			{
				case VAR_USHORT:

					if (opacity > 0.0)
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(void*) (display->getBegin() * sizeof(short)));

					if (wireframe)
					{
						render->disableBlending();
						opacity = 1.0;

						texturePasses = 0;
						render->sendUniformInt(m_fx[0], "TextureMode", &texturePasses);
						render->sendUniformFloat(m_fx[0], "Opacity", &opacity);
						render->sendUniformVec3(m_fx[0], "Diffuse", Vector3(0, 0, 0));
						render->enablePolygonOffset(-0.25, -1);

						render->setPolygonMode(PRIMITIVE_LINES);
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(void*) (display->getBegin() * sizeof(short)));
						render->setPolygonMode(PRIMITIVE_TRIANGLES);

						render->disablePolygonOffset();
					}
					break;
				case VAR_UINT:
					if (opacity > 0.0)
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(void*) (display->getBegin() * sizeof(int)));

					if (wireframe)
					{
						render->disableBlending();
						render->sendUniformVec3(m_fx[0], "Diffuse", Vector3(0, 0, 0));

						opacity = 1.0;
						render->sendUniformFloat(m_fx[0], "Opacity", &opacity);
						texturePasses = 0;
						render->sendUniformInt(m_fx[0], "TextureMode", &texturePasses);

						render->enablePolygonOffset(-0.25, -1);

						render->setPolygonMode(PRIMITIVE_LINES);
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(void*) (display->getBegin() * sizeof(int)));
						render->setPolygonMode(PRIMITIVE_TRIANGLES);

						render->disablePolygonOffset();
					}
					break;
				default:
				MLOG_WARNING("Unsupported indices type!");
			}
		}
		else // If the indices are not stored in the VBO
		{
			switch (indicesType)
			{
				case VAR_USHORT:
					if (opacity > 0.0)
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(unsigned short*) indices + display->getBegin());

					if (wireframe)
					{
						render->disableBlending();
						render->sendUniformVec3(m_fx[0], "Diffuse", Vector3(0, 0, 0));

						opacity = 1.0;
						render->sendUniformFloat(m_fx[0], "Opacity", &opacity);
						texturePasses = 0;
						render->sendUniformInt(m_fx[0], "TextureMode", &texturePasses);

						render->enablePolygonOffset(-0.25, -1);

						render->setPolygonMode(PRIMITIVE_LINES);
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(unsigned short*) indices + display->getBegin());
						render->setPolygonMode(PRIMITIVE_TRIANGLES);

						render->disablePolygonOffset();
					}
					break;
				case VAR_UINT:
					if (opacity > 0.0)
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(unsigned int*) indices + display->getBegin());

					if (wireframe)
					{
						render->disableBlending();
						render->sendUniformVec3(m_fx[0], "Diffuse", Vector3(0, 0, 0));

						opacity = 1.0;
						render->sendUniformFloat(m_fx[0], "Opacity", &opacity);
						texturePasses = 0;
						render->sendUniformInt(m_fx[0], "TextureMode", &texturePasses);

						render->enablePolygonOffset(-0.25, -1);

						render->setPolygonMode(PRIMITIVE_LINES);
						render->drawElement(display->getPrimitiveType(),
											display->getSize(),
											indicesType,
											(unsigned int*) indices + display->getBegin());
						render->setPolygonMode(PRIMITIVE_TRIANGLES);

						render->disablePolygonOffset();
					}
					break;
				default:
				MLOG_WARNING("Unsupported indices type!");
			}
		}
	}
	else // If we have no indices
	if (opacity > 0.0) render->drawArray(display->getPrimitiveType(), display->getBegin(), display->getSize());
}

void StandardRenderer::drawGBuffer(Scene* scene, OCamera* camera)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	render->setDepthMask(true);

	OEntity* entity;
	Mesh* mesh;

	// Lock scene so we don't have a surprise
	CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());
	if (!data) return;

	for (OEntity* entity : data->visibleEntities)
	{
		drawEntity(entity, camera);
	}

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();
}

void StandardRenderer::drawEntity(OEntity* entity, OCamera* camera)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	if (entity->isVisible() && entity->isActive() && !entity->isInvisible())
	{
		render->setMatrixMode(MATRIX_MODELVIEW);
		render->loadIdentity();
		render->multMatrix(entity->getMatrix());

		Mesh* mesh = entity->getMesh();

		// Update Animation
		if (mesh->getArmatureAnim())
		{
			animateArmature(
				mesh->getArmature(),
				mesh->getArmatureAnim(),
				entity->getCurrentFrame()
			);
		}
		else if (mesh->getArmature())
		{
			mesh->getArmature()->processBonesLinking();
			mesh->getArmature()->updateBonesSkinMatrix();
		}

		if (mesh->getMaterialsAnim())
			animateMaterials(mesh, mesh->getMaterialsAnim(), entity->getCurrentFrame());

		if (mesh->getTexturesAnim())
			animateTextures(mesh, mesh->getTexturesAnim(), entity->getCurrentFrame());

		updateSkinning(entity->getMesh(), mesh->getArmature());
		drawMesh(entity->getMesh(), camera, entity->getMaterial(), entity->hasWireframe());
	}
}

void StandardRenderer::drawTransparents(Scene* scene, OCamera* camera)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();
	Mesh* mesh;

	// Lock scene so we don't have a surprise
	CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());

	if (!data) return;

	sendLights(m_fx[0], camera);
	render->bindTexture(m_depthTexID, 4);

	for (OEntity* entity : data->visibleTransparentEntities)
	{
		drawEntity(entity, camera);
	}

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();

	int num = scene->getTextsNumber();
	for (int i = 0; i < num; i++)
	{
		drawText(scene->getTextByIndex(i), camera);
	}
}

void StandardRenderer::initialize()
{
	std::string path;
	Level* level = NeoEngine::getInstance()->getLevel();
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	// Do not initialize twice
	if (m_fx[0] != -1 && m_fx[1] != -1)
		return;

	MLOG_INFO("Initializing the rendering engine");
	render->init();

	path = system->getWorkingDirectory();
	path += "/";
	path += SHADER_PATH;

	char file[512];

	getGlobalFilename(file, path.c_str(), defaultShaderFiles[0]);
	ShaderRef* vertShad = level->loadShader(file, M_SHADER_VERTEX);
	ShaderRef* fragShad;

	vertShad->update();

	unsigned int id = 0;
	for (int i = 1; i < NUM_SHADERS; i++)
	{
		MLOG_INFO("Loading shader: " << defaultShaderFiles[i]);
		getGlobalFilename(file, path.c_str(), defaultShaderFiles[i]);

		fragShad = level->loadShader(file, M_SHADER_PIXEL);
		fragShad->update();

		m_shaders[i - 1] = level->createFX(vertShad, fragShad);
		render->updateFX(m_shaders[i - 1]->getFXId());

		m_shaders[i - 1]->setImportant(true);
		m_fx[i - 1] = m_shaders[i - 1]->getFXId();

		// Clean up afterwards
		// TODO: Clear the shader manager to free all remaining resources!
		id = fragShad->getShaderId();
		render->deleteShader(&id, m_fx[i - 1]);
		fragShad->setShaderId(0);
	}

	id = vertShad->getShaderId();
	for (int i = 0; i < NUM_SHADERS; i++)
	{
		render->deleteShader(&id, m_fx[i]);
	}

	initFramebuffers();
	initQuadVAO(&m_quadVAO, &m_quadVBO, &m_quadTexCoordVBO, m_fx[1]);
	initTextVAO(&m_textVAO, &m_textVBO, &m_textTexCoordVBO, m_fx[0]);

	// Generate some random data
	Image random;
	random.create(VAR_UBYTE, 128, 128, 4);
	
	unsigned char* p = static_cast<unsigned char*>(random.getData());
	for(unsigned int i = 0; i < random.getSize(); i++)
	{
		(*p) = static_cast<unsigned char>(rand() % 256);
	    p++;
	}

	m_randomTexID = 0;
	render->createTexture(&m_randomTexID);
	render->bindTexture(m_randomTexID);
	render->setTextureFilterMode(TEX_FILTER_LINEAR_MIPMAP_LINEAR, TEX_FILTER_LINEAR);
	render->setTextureUWrapMode(WRAP_REPEAT);
	render->setTextureVWrapMode(WRAP_REPEAT);
	render->sendTextureImage(&random, 1, 1, 0);
}

void StandardRenderer::smallInit(unsigned int width, unsigned int height)
{
	std::string path;
	Level* level = NeoEngine::getInstance()->getLevel();
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	render->init();

	path = system->getWorkingDirectory();
	path += "/";
	path += SHADER_PATH;

	char file[512];

	getGlobalFilename(file, path.c_str(), defaultShaderFiles[0]);
	ShaderRef* vertShad = level->loadShader(file, M_SHADER_VERTEX);
	ShaderRef* fragShad;

	vertShad->update();

	for (int i = 1; i < NUM_SHADERS; i++)
	{
		getGlobalFilename(file, path.c_str(), defaultShaderFiles[i]);
		fragShad = level->loadShader(file, M_SHADER_PIXEL);
		fragShad->update();

		m_shaders[i - 1] = level->createFX(vertShad, fragShad);
		render->updateFX(m_shaders[i - 1]->getFXId());

		m_shaders[i - 1]->setImportant(true);
		m_fx[i - 1] = m_shaders[i - 1]->getFXId();
	}

	initFramebuffers(Vector2(width, height));
	initQuadVAO(&m_quadVAO, &m_quadVBO, &m_quadTexCoordVBO, m_fx[1]);
	initTextVAO(&m_textVAO, &m_textVBO, &m_textTexCoordVBO, m_fx[0]);
}

inline int Pow2(int x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

void StandardRenderer::KillThreads()
{
	return;
	if (m_lightUpdateThread == NULL || m_visibilityThread == NULL)
		return;

	startThreads();

	m_threadExit = true;

	m_lightUpdateThread->WaitForReturn();
	m_visibilityThread->WaitForReturn();

	m_threadExit = false;
}

void StandardRenderer::clearFramebuffers()
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	if (m_gbufferTexID != 0)
		render->deleteTexture(&m_gbufferTexID);

	if (m_depthTexID != 0)
		render->deleteTexture(&m_depthTexID);

	if (m_dataTexID != 0)
		render->deleteTexture(&m_dataTexID);

	if (m_framebufferID != 0)
		render->deleteFrameBuffer(&m_framebufferID);
}

void StandardRenderer::initFramebuffers(Vector2 res)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();

	clearFramebuffers();

	// screen size
	if (res.x == 0 || res.y == 0)
	{
		unsigned int screenWidth = 0;
		unsigned int screenHeight = 0;
		system->getScreenSize(&screenWidth, &screenHeight);
		res = Vector2(screenWidth, screenHeight);
	}

	m_resolution = Vector2(res.x, res.y);

	// Update vertex cache
	m_vertices[0] = Vector2(0, 0);
	m_vertices[1] = Vector2(0, res.y);
	m_vertices[2] = Vector2(res.x, 0);
	m_vertices[3] = Vector2(res.x, res.y);

	// create frame buffer
	render->createFrameBuffer(&m_framebufferID);

	// create render textures
	// GBuffer texture
	render->createTexture(&m_gbufferTexID);
	render->bindTexture(m_gbufferTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_UBYTE, TEX_RGBA, 0);

	// Depth texture
	render->createTexture(&m_depthTexID);
	render->bindTexture(m_depthTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_UBYTE, TEX_DEPTH, 0);

	// Position texture
	render->createTexture(&m_positionTexID);
	render->bindTexture(m_positionTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_FLOAT, TEX_RGBA, 0);

	// Normal texture
	render->createTexture(&m_normalTexID);
	render->bindTexture(m_normalTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_FLOAT, TEX_RGBA, 0);

	// Data texture
	render->createTexture(&m_dataTexID);
	render->bindTexture(m_dataTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_UBYTE, TEX_RGBA, 0);

	render->bindFrameBuffer(m_framebufferID);

	render->attachFrameBufferTexture(ATTACH_COLOR0, m_gbufferTexID);
	render->attachFrameBufferTexture(ATTACH_COLOR1, m_normalTexID);
	render->attachFrameBufferTexture(ATTACH_COLOR2, m_positionTexID);
	render->attachFrameBufferTexture(ATTACH_COLOR3, m_dataTexID);
	// render->attachFrameBufferTexture(ATTACH_COLOR4, m_depthTexID);
	render->attachFrameBufferTexture(ATTACH_DEPTH, m_depthTexID);

	// Enable them to be drawn
	FRAME_BUFFER_ATTACHMENT buffers[5] = {ATTACH_COLOR0, ATTACH_COLOR1, ATTACH_COLOR2, ATTACH_COLOR3, ATTACH_COLOR4};
	render->setDrawingBuffers(buffers, 4);
	render->bindFrameBuffer(0);
	render->bindTexture(0);

	// Prepare post process fbo
	render->createFrameBuffer(&m_finalFramebufferID);
	render->bindFrameBuffer(m_finalFramebufferID);

	render->createTexture(&m_finalTexID);
	render->bindTexture(m_finalTexID);
	render->setTextureFilterMode(TEX_FILTER_NEAREST, TEX_FILTER_NEAREST);
	render->setTextureUWrapMode(WRAP_CLAMP);
	render->setTextureVWrapMode(WRAP_CLAMP);
	render->texImage(0, res.x, res.y, VAR_UBYTE, TEX_RGBA, 0);

	render->attachFrameBufferTexture(ATTACH_COLOR0, m_finalTexID);
	render->attachFrameBufferTexture(ATTACH_DEPTH, m_depthTexID);

	FRAME_BUFFER_ATTACHMENT finalbuffers[1] = {ATTACH_COLOR0};
	render->setDrawingBuffers(finalbuffers, 1);

	render->bindFrameBuffer(0);
}

void StandardRenderer::destroy(void)
{
	KillThreads();
	delete this;
}

void StandardRenderer::updateSkinning(Mesh* mesh, Armature* armature)
{
	unsigned int s;
	unsigned int size = mesh->getSubMeshsNumber();
	for (s = 0; s < size; s++)
	{
		SubMesh* subMesh = &mesh->getSubMeshs()[s];
		Vector3* vertices = subMesh->getVertices();

		if (!vertices)
			continue;

		SkinData* skinData = subMesh->getSkinData();
		if (armature && skinData)
		{
			unsigned int verticesSize = subMesh->getVerticesSize();
			Vector3* skinVertices = subMesh->getSkinVertices();

			computeSkinning(armature, skinData, vertices, NULL, NULL, skinVertices, NULL, NULL);
			subMesh->getBoundingBox()->initFromPoints(skinVertices, verticesSize);
		}
	}

	mesh->updateBoundingBox();
}

void StandardRenderer::prepareMaterialDisplay(SubMesh* mesh, MaterialDisplay* display)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	unsigned int* vao = display->getVAO();
	unsigned int* vboId1 = mesh->getVBOid1();
	unsigned int* vboId2 = mesh->getVBOid2();
	void* indices = mesh->getIndices();
	Vector2* texcoords = mesh->getTexCoords();

	render->bindFX(m_fx[0]);

	if (*vao == 0)
		render->createVAO(vao);

	render->bindVAO(*vao);

	// Bind VBOs
	if (*vboId1 > 0)
		render->bindVBO(VBO_ARRAY, *vboId1);

	if (indices) // If the SubMesh has indices
	{
		if (*vboId2 > 0) // If the indices are stored in the VBO
		{
			render->bindVBO(VBO_ELEMENT_ARRAY, *vboId2);
		}
	}

	// Set up vertex attribute
	int vertAttrib;
	int offset = 0;

	render->getAttribLocation(m_fx[0], "Vertex", &vertAttrib);
	render->enableAttribArray(vertAttrib);
	render->setAttribPointer(vertAttrib, VAR_FLOAT, 3, NULL);

	offset = sizeof(Vector3) * (mesh->getVerticesSize() + display->getBegin());

	// Set up normal attribute
	int normalAttrib;
	render->getAttribLocation(m_fx[0], "Normal", &normalAttrib);
	render->enableAttribArray(normalAttrib);
	render->setAttribPointer(normalAttrib, VAR_FLOAT, 3, (void*) offset);

	offset += sizeof(Vector3) * mesh->getNormalsSize();

	// Set up tangent attribute
	int tangentAttrib;
	render->getAttribLocation(m_fx[0], "Tangent", &tangentAttrib);
	render->enableAttribArray(tangentAttrib);
	render->setAttribPointer(tangentAttrib, VAR_FLOAT, 3, (void*) offset);

	offset += sizeof(Vector3) * mesh->getTangentsSize();

	// Set up color attribute
	int colorAttrib;
	render->getAttribLocation(m_fx[0], "Color", &colorAttrib);
	render->enableAttribArray(colorAttrib);
	render->setAttribPointer(colorAttrib, VAR_FLOAT, 3, (void*) offset);

	// Set up texcoord attribute
	int texcoordAttrib = 0;
	if (display->getMaterial()->getTexturesPassNumber() > 0 && texcoords != NULL)
	{
		// vert + normal + tang
		offset = sizeof(Vector3)
			* (mesh->getVerticesSize() + mesh->getNormalsSize() + mesh->getTangentsSize() + display->getBegin());

		render->getAttribLocation(m_fx[0], "TexCoord", &texcoordAttrib);
		render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, (void*) offset);
		render->enableAttribArray(texcoordAttrib);
	}

	render->bindVAO(0);
}

void StandardRenderer::drawToTexture(Scene* scene, OCamera* camera, unsigned int texId)
{
}

void StandardRenderer::renderShadows(Scene* scene, OCamera* maincam)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	CameraData* camData = static_cast<CameraData*>(maincam->getAdditionalData());
	if (!camData) return;

	unsigned int currentFrameBuffer = 0;
	render->getCurrentFrameBuffer(&currentFrameBuffer);

	for (OLight* light : camData->visibleLights)
	{
		if (light->isCastingShadow())
		{
			LightData* ld = static_cast<LightData*> (light->getAdditionalData());
			if (!ld)
			{
				MLOG_INFO ("Creating light data!");
				ld = new LightData();
				light->setAdditionalData(ld);
			}

			if (ld->resolution != light->getShadowQuality())
			{
				if (ld->depthTexture)
					render->deleteTexture(&ld->depthTexture);

				if (ld->fbo)
					render->deleteFrameBuffer(&ld->fbo);

				ld->resolution = light->getShadowQuality();

				render->createFrameBuffer(&ld->fbo);
				render->createTexture(&ld->depthTexture);

				render->bindTexture(ld->depthTexture);
				render->texImage(0, ld->resolution, ld->resolution, VAR_FLOAT, TEX_DEPTH, NULL);

				render->bindFrameBuffer(ld->fbo);
				render->attachFrameBufferTexture(ATTACH_DEPTH, ld->depthTexture);
			}

			ld->camera.setClippingFar(light->getRadius());

			if (light->getSpotAngle() == 0)
				ld->camera.enableOrtho(true);
			else if (light->getSpotAngle() < 180)
				ld->camera.enableOrtho(false);
			else
				return;

			*ld->camera.getMatrix() = *light->getMatrix();
			ld->camera.setFov(light->getSpotAngle());

			drawScene(scene, &ld->camera);
			render->bindFrameBuffer(0);
		}
		else if (light->getAdditionalData())
		{
			delete light->getAdditionalData();
			light->setAdditionalData(NULL);

			MLOG_INFO("Deleting light data!");
		}
	}

	render->bindFrameBuffer(currentFrameBuffer);
}

void StandardRenderer::drawScene(Scene* scene, OCamera* camera)
{
	//drawToTexture(scene, camera, m_gbufferTexID);
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();
	SystemContext* system = engine->getSystemContext();

	// Culling
	{
		Vector3 campos = camera->getTransformedPosition();
		CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());
		if (!data) camera->setAdditionalData((data = new CameraData));

		data->visibleEntities.clear();
		data->visibleTransparentEntities.clear();

		camera->enable();
		camera->getFrustum()->makeVolume(camera);

		data->visibleEntities.reserve(scene->getEntitiesNumber());
		data->visibleTransparentEntities.reserve(scene->getEntitiesNumber());

		PROFILE_BEGIN("Culling")

//#pragma omp parallel for
		for (int i = 0; i < scene->getEntitiesNumber(); i++)
		{
			OEntity* e = scene->getEntityByIndex(i);
			e->updateVisibility(camera);

			if (e->isVisible())
			{
				if (e->hasTransparency())
				{
					Vector3 epos = e->getTransformedPosition();

					auto i = data->visibleTransparentEntities.begin();
					while(i != data->visibleTransparentEntities.end()
							&& (epos - campos).getLength()
							< (epos - (*i)->getTransformedPosition()).getLength()) ++i;

					data->visibleTransparentEntities.push_back(e);
				}
				else
				{
					data->visibleEntities.push_back(e);
				}
			}
		}

		PROFILE_END("Culling");

		data->visibleLights.clear();
		for (int i = 0, j = 0; i < scene->getLightsNumber(); i++)
		{
			OLight* light;
			// If light is visible
			if ((light = scene->getLightByIndex(i)) != NULL && light->isActive()
				&& (light->isVisible() || light->getSpotAngle() == 0))
			{
				data->visibleLights.push_back(light);
				j++;
			}
		}
	}

	unsigned int currentFrameBuffer = 0;
	render->getCurrentFrameBuffer(&currentFrameBuffer);

	// screen size
	unsigned int screenWidth = 0;
	unsigned int screenHeight = 0;
	system->getScreenSize(&screenWidth, &screenHeight);

	if(Vector2(screenWidth, screenHeight) != m_resolution)
	{
		initFramebuffers();
		initQuadVAO(&m_quadVAO, &m_quadVBO, &m_quadTexCoordVBO, m_fx[1]);
	}

	//renderShadows(scene, camera);

	// render to texture
	render->bindFrameBuffer(m_framebufferID);
	//render->setViewport(0, 0, screenWidth, screenHeight); // change viewport

	Vector4 clearColor = camera->getClearColor();

	clearColor.w = 0.0;
	render->setClearColor(clearColor);

	// This prevents the clear color from bleeding over multiple
	// scene layers
	render->setColorMask(false, false, false, true);
	render->clear(BUFFER_COLOR | BUFFER_DEPTH);
	render->setColorMask(true, true, true, true);

	// Send some common data
	Vector3 ambientLight = scene->getAmbientLight();
	render->sendUniformVec3(m_fx[1], "AmbientLight", ambientLight);

	sendLights(m_fx[1], camera);
	drawGBuffer(scene, camera);

	// finish render to texture
	render->bindFrameBuffer(m_finalFramebufferID);
	renderFinalImage(scene, camera);

	drawTransparents(scene, camera);

	render->bindFrameBuffer(currentFrameBuffer);
	renderFinalImage(scene, camera, true);

	render->bindVAO(0);
}

void StandardRenderer::sendLights(unsigned int shader, OCamera* camera)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	render->bindFX(shader);

	Matrix4x4 camMat;
	camera->enable();
	camMat = *camera->getCurrentViewMatrix();

	// Send light data
	CameraData* data = static_cast<CameraData*>(camera->getAdditionalData());
	if (!data) return;

	int num = data->visibleLights.size();
	render->sendUniformInt(shader, "LightsCount", &num);

	size_t size = getNextPowerOfTwo(data->visibleLights.size());
	if (data->lightData.getWidth() < size
		|| data->lightData.getHeight() == 0)
	{
		// Texture format:
		// x ... size => Light index
		// tex[x][0] => position
		// tex[x][1].xyz => spot cos, spot exp, intensity
		// tex[x][2].xyz => radius
		// tex[x][3].xyz => linear attenuation, quadratic attenuation, constant attenuation
		// tex[x][4] => spot dir
		// tex[x][5] => diffuse
		data->lightData.create(VAR_FLOAT, size, 8, 3);
	}

	Vector3 vec;
	int i = 0;
	for (OLight* l : data->visibleLights)
	{
		float quadraticAtten = (8.0f / l->getRadius());
		quadraticAtten = (quadraticAtten * quadraticAtten) * l->getIntensity();
		float spotCos = cosf(l->getSpotAngle() * DEG_TO_RAD);

		vec = camMat * l->getTransformedPosition();
		data->lightData.writePixel(i, 0, &vec);

		vec = Vector3(spotCos, l->getSpotExponent(), l->getIntensity());
		data->lightData.writePixel(i, 1, &vec);

		vec = Vector3(l->getRadius(), 0, 0);
		data->lightData.writePixel(i, 2, &vec);

		vec = Vector3(0, quadraticAtten, 1.0f);
		data->lightData.writePixel(i, 3, &vec);

		vec = camMat.getRotatedVector3(l->getRotatedVector(Vector3(0, 0, -1))).getNormalized();
		data->lightData.writePixel(i, 4, &vec);

		vec = l->getColor();
		data->lightData.writePixel(i, 5, &vec);
		
		i++;
	}

	num = 7;

	// FIXME: Do that in the constructor!
	if (!data->lightTexture)
		render->createTexture(&data->lightTexture);

	render->bindTexture(data->lightTexture, 7);
	render->sendTextureImage(&data->lightData, false, false, false);
	render->sendUniformInt(shader, "LightData", &num);
}

void StandardRenderer::sendLight(unsigned int fx, OLight* l, int num, Matrix4x4 matrix)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	char str[255];
	char ending[255];

	sprintf(str, "lights[%d].", num);

	strcpy(ending, str);
	strcat(ending, "Position");
	Vector3 position = matrix * l->getTransformedPosition();
	render->sendUniformVec3(fx, ending, position);

	strcpy(ending, str);
	strcat(ending, "Diffuse");
	render->sendUniformVec3(fx, ending, l->getFinalColor());

	float intensity = l->getIntensity();
	strcpy(ending, str);
	strcat(ending, "Intensity");
	render->sendUniformFloat(fx, ending, &intensity);

	float radius = l->getRadius();
	strcpy(ending, str);
	strcat(ending, "Radius");
	render->sendUniformFloat(fx, ending, &radius);

	float spotAngle = cosf(l->getSpotAngle() * DEG_TO_RAD);
	strcpy(ending, str);
	strcat(ending, "SpotCos");
	render->sendUniformFloat(fx, ending, &spotAngle);

	float spotExponent = l->getSpotExponent();
	strcpy(ending, str);
	strcat(ending, "SpotExp");
	render->sendUniformFloat(fx, ending, &spotExponent);

	strcpy(ending, str);
	strcat(ending, "SpotDir");
	Vector3 spotDirection = matrix.getRotatedVector3(l->getRotatedVector(Vector3(0, 0, -1))).getNormalized();
	render->sendUniformVec3(fx, ending, spotDirection);

	float quadraticAttenuation = 0.0;
	// attenuation
	//if (l->getSpotAngle() > 0.0f)
	//{
	quadraticAttenuation = (8.0f / l->getRadius());
	quadraticAttenuation = (quadraticAttenuation * quadraticAttenuation) * l->getIntensity();
	//}

	strcpy(ending, str);
	strcat(ending, "QuadraticAttenuation");
	render->sendUniformFloat(fx, ending, &quadraticAttenuation);

	// Constant attenuation = 1
	float attenuation = 1.0;
	strcpy(ending, str);
	strcat(ending, "ConstantAttenuation");
	render->sendUniformFloat(fx, ending, &attenuation);

	// Linear attenuation = 0
	attenuation = 0.0;
	strcpy(ending, str);
	strcat(ending, "LinearAttenuation");
	render->sendUniformFloat(fx, ending, &attenuation);

	if (l->isCastingShadow())
	{
		strcpy(ending, str);
		strcat(ending, "ShadowTexture");

		LightData* data = static_cast<LightData*>(l->getAdditionalData());

		int nil = 0;
		if (data)
			render->sendUniformInt(fx, ending, (int*) &data->depthTexture);
		else
			render->sendUniformInt(fx, ending, &nil);

		strcpy(ending, str);
		strcat(ending, "ShadowMatrix");
		render->sendUniformMatrix(fx, ending, l->getMatrix());
	}
}

void StandardRenderer::renderFinalImage(Scene* scene, OCamera* camera, bool postFx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	SystemContext* system = NeoEngine::getInstance()->getSystemContext();

	// screen size
	unsigned int screenWidth = 0;
	unsigned int screenHeight = 0;
	system->getScreenSize(&screenWidth, &screenHeight);

	// draw the rendered textured with a shader effect
	set2D(screenWidth, screenHeight);

	render->bindFX(m_fx[1]);
	int postEffects = 1;

	if (!postFx)
	{
		postEffects = 0;
		render->bindTexture(m_gbufferTexID);

		render->enableBlending();
		render->setBlendingMode(BLENDING_ALPHA);

		render->sendUniformVec3(m_fx[1], "AmbientLight", scene->getAmbientLight() * 0.1);

		// This prevents the clear color from bleeding over multiple
		// scene layers
		render->setColorMask(false, false, false, true);
		render->clear(BUFFER_COLOR);
		render->setColorMask(true, true, true, true);
	}
	else
	{
		render->enableBlending();
		render->bindTexture(m_finalTexID);

		float val = camera->getClippingNear();
		render->sendUniformFloat(m_fx[1], "Near", &val, 1);

		val = camera->getClippingFar();
		render->sendUniformFloat(m_fx[1], "Far", &val, 1);

		val = system->getSystemTick();
		render->sendUniformMatrix(m_fx[1], "CameraViewMatrix", camera->getCurrentViewMatrix(), 1);
		render->sendUniformFloat(m_fx[1], "Time", &val, 1);
	}

	render->bindTexture(m_normalTexID, 1);
	render->bindTexture(m_positionTexID, 2);
	render->bindTexture(m_dataTexID, 3);

	render->sendUniformInt(m_fx[1], "PostEffects", &postEffects, 1);
	render->bindTexture(m_depthTexID, 4);
	render->bindTexture(m_randomTexID, 5);
	
	int texIds[6] = {0, 1, 2, 3, 4, 5};
	render->sendUniformInt(m_fx[1], "Textures", texIds, 6);

	// Set cull mode
	render->setCullMode(CULL_BACK);
	render->disableDepthTest();
	render->setDepthMask(false);

	drawQuad(m_fx[1]);

	render->bindFX(0);
	render->enableDepthTest();
	render->setDepthMask(true);
}

void StandardRenderer::drawText(OText* textObj, OCamera* camera)
{
	const char* text = textObj->getText();

	if (!text || strlen(text) == 0)
		return;

	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	Font* font = textObj->getFont();

	Matrix4x4 modelViewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 mvp;

	if (camera != nullptr)
	{
		modelViewMatrix =
			(*camera->getCurrentViewMatrix()) * (*textObj->getMatrix());
		projectionMatrix = *camera->getCurrentProjMatrix();
	}
	else
	{
		render->getModelViewMatrix(&modelViewMatrix);
		render->getProjectionMatrix(&projectionMatrix);
	}

	mvp = projectionMatrix * modelViewMatrix;

	render->bindFX(m_fx[0]);
	render->bindVAO(m_textVAO);

	render->selectSubroutine(m_fx[0], M_SHADER_PIXEL, "TextShader");
	render->bindTexture(font->getTextureId());

	render->enableBlending();
	render->setBlendingMode(BLENDING_ALPHA);
	//render->enableDepthTest();
	//render->disableCullFace();

	int uniform = 1;
	render->sendUniformInt(m_fx[0], "HasTransparency", &uniform);

	float opacity = textObj->getColor().w;
	render->sendUniformFloat(m_fx[0], "Opacity", &opacity);

	uniform = -1;
	render->sendUniformInt(m_fx[0], "TextureMode", &uniform);

	int texIds[5] = {0, 1, 2, 3, 4};
	render->sendUniformInt(m_fx[0], "Textures", texIds, 5);
	render->sendUniformMatrix(m_fx[0], "ProjModelViewMatrix", &mvp);

	render->sendUniformVec3(m_fx[0], "Diffuse", textObj->getColor());

	Character* c;
	Vector2 scale;
	Vector2 pos;

	float fontSize = font->getFontSize();
	float size = textObj->getSize();
	float tabsize = size * 2.0f;
	float widthFactor = font->getTextureWidth() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;

	float xpos = 0;
	float ypos = 0;

	Vector2 texCoords[4];
	Vector2 vertices[4];
	Vector2 offset;

	vector<float>* linesOffset = textObj->getLinesOffset();

	if (!linesOffset || linesOffset->size() == 0)
		return;

	float lineOffset = (*linesOffset)[0];
	int line = 0;

	unsigned int state = 0;
	unsigned int character;
	unsigned char* s = (unsigned char*) text;

	for (; *s; s++)
	{
		if (utf8_decode(&state, &character, *s) != UTF8_ACCEPT)
			continue;

		if (character == '\n')
		{
			line++;
			lineOffset = (*linesOffset)[line];

			xpos = 0;
			ypos += size;
			continue;
		}

		if (character == '\t')
		{
			unsigned int tab = static_cast<unsigned int>(xpos / tabsize) + 1;
			xpos = tab * tabsize;
			continue;
		}

		c = font->getCharacter(character);

		if (!c)
			continue;

		pos = c->getPos();
		scale = c->getScale();
		offset = c->getOffset() * size;

		// Round to keep the text pixel aligned when doing
		// 2D rendering. Should not affect 3D text much.
		offset.x += floor(lineOffset);

		texCoords[0] = Vector2(pos.x, pos.y + scale.y);
		texCoords[1] = Vector2(pos.x + scale.x, pos.y + scale.y);
		texCoords[2] = Vector2(pos.x, pos.y);
		texCoords[3] = Vector2(pos.x + scale.x, pos.y);

		float width = scale.x * widthFactor * size;
		float height = scale.y * heightFactor * size;

		vertices[0] = Vector2(xpos, ypos + height) + offset;
		vertices[1] = Vector2(xpos + width, ypos + height) + offset;
		vertices[2] = Vector2(xpos, ypos) + offset;
		vertices[3] = Vector2(xpos + width, ypos) + offset;

		// TexCoord
		render->bindVBO(VBO_ARRAY, m_textTexCoordVBO);
		render->setVBO(VBO_ARRAY, texCoords, 4 * sizeof(Vector2), VBO_DYNAMIC);

		// Vertices
		render->bindVBO(VBO_ARRAY, m_textVBO);
		render->setVBO(VBO_ARRAY, vertices, 4 * sizeof(Vector2), VBO_DYNAMIC);

		render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);
		xpos += c->getXAdvance() * size;
	}

	render->bindVAO(0);
}

void StandardRenderer::initVBO(SubMesh* subMesh)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	unsigned int* vboId1 = subMesh->getVBOid1();
	unsigned int* vboId2 = subMesh->getVBOid2();

	VBO_MODES mode = VBO_STATIC;
	if (subMesh->getSkinData() || subMesh->getMorphingData())
		mode = VBO_DYNAMIC;

	if (*vboId1 == 0 || mode == VBO_DYNAMIC)
	{
		// data
		Color* colors = subMesh->getColors();

		Vector3* vertices = subMesh->getVertices();
		Vector3* normals = subMesh->getNormals();
		Vector3* tangents = subMesh->getTangents();

		Vector2* texCoords = subMesh->getTexCoords();

		if (mode == VBO_DYNAMIC)
		{
			vertices = subMesh->getSkinVertices();
			normals = subMesh->getSkinNormals();
			tangents = subMesh->getSkinTangents();
		}

		unsigned int totalSize = sizeof(Vector3) * subMesh->getVerticesSize();
		if (normals)
			totalSize += sizeof(Vector3) * subMesh->getNormalsSize();
		if (tangents)
			totalSize += sizeof(Vector3) * subMesh->getTangentsSize();
		if (texCoords)
			totalSize += sizeof(Vector2) * subMesh->getTexCoordsSize();
		if (colors)
			totalSize += sizeof(Color) * subMesh->getColorsSize();

		// indices
		VAR_TYPES indicesType = subMesh->getIndicesType();
		void* indices = subMesh->getIndices();

		// data VBO
		if (*vboId1 == 0)
			render->createVBO(vboId1);

		render->bindVBO(VBO_ARRAY, *vboId1);
		render->setVBO(VBO_ARRAY, 0, totalSize, mode);

		unsigned int offset = 0;
		render->setVBOSubData(VBO_ARRAY, offset, vertices, sizeof(Vector3) * subMesh->getVerticesSize());
		offset += sizeof(Vector3) * subMesh->getVerticesSize();

		if (normals)
		{
			render->setVBOSubData(VBO_ARRAY, offset, normals, sizeof(Vector3) * subMesh->getNormalsSize());
			offset += sizeof(Vector3) * subMesh->getNormalsSize();
		}

		if (tangents)
		{
			render->setVBOSubData(VBO_ARRAY, offset, tangents, sizeof(Vector3) * subMesh->getTangentsSize());
			offset += sizeof(Vector3) * subMesh->getTangentsSize();
		}

		if (texCoords)
		{
			render->setVBOSubData(VBO_ARRAY, offset, texCoords, sizeof(Vector2) * subMesh->getTexCoordsSize());
			offset += sizeof(Vector2) * subMesh->getTexCoordsSize();
		}

		if (colors)
		{
			render->setVBOSubData(VBO_ARRAY, offset, colors, sizeof(Color) * subMesh->getColorsSize());
			offset += sizeof(Color) * subMesh->getColorsSize();
		}

		// indices VBO
		if (indices)
		{
			unsigned int typeSize = indicesType == VAR_USHORT ? sizeof(short) : sizeof(int);

			if (*vboId2 == 0)
				render->createVBO(vboId2);

			render->bindVBO(VBO_ELEMENT_ARRAY, *vboId2);
			render->setVBO(VBO_ELEMENT_ARRAY, indices, subMesh->getIndicesSize() * typeSize, mode);
		}

		render->bindVBO(VBO_ARRAY, 0);
		render->bindVBO(VBO_ELEMENT_ARRAY, 0);
	}
}

void StandardRenderer::set2D(unsigned int w, unsigned int h)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	render->setViewport(0, 0, w, h);

	// set ortho projection
	render->setMatrixMode(MATRIX_PROJECTION);
	render->loadIdentity();

	render->setOrthoView(0.0f, (float) w, (float) h, 0.0f, 1.0f, -1.0f);

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();
}

void StandardRenderer::drawQuad(int fx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	// screen size
	int screenWidth = 0;
	int screenHeight = 0;
	NeoEngine::getInstance()->getSystemContext()
		->getScreenSize((unsigned int*) &screenWidth, (unsigned int*) &screenHeight);

	// projmodelview matrix
	static Matrix4x4 ProjMatrix;
	static Matrix4x4 ModelViewMatrix;
	static Matrix4x4 ProjModelViewMatrix;

	render->getProjectionMatrix(&ProjMatrix);
	render->getModelViewMatrix(&ModelViewMatrix);
	ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
	render->sendUniformMatrix(fx, "ProjModelViewMatrix", &ProjModelViewMatrix);

	// Texture
	int texIds[4] = {0, 1, 2, 3};
	render->sendUniformInt(fx, "Textures", texIds, 4);

	// Width
	render->sendUniformInt(fx, "Width", &screenWidth, 1);
	// Height
	render->sendUniformInt(fx, "Height", &screenHeight, 1);

	// draw
	render->bindVAO(m_quadVAO);
	render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);
}

void StandardRenderer::initQuadVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo, unsigned int fx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	if (*vao || *vbo || *texcoordVbo)
		clearQuadVAO(vao, vbo, texcoordVbo);

	render->createVAO(vao);
	render->bindVAO(*vao);

	render->createVBO(vbo);
	render->bindVBO(VBO_ARRAY, *vbo);
	render->setVBO(VBO_ARRAY, m_vertices, 4 * sizeof(Vector2), VBO_STATIC);

	render->createVBO(texcoordVbo);
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->setVBO(VBO_ARRAY, m_texCoords, 4 * sizeof(Vector2), VBO_STATIC);

	// Send Vertex data
	int vertexAttrib;
	int texcoordAttrib;

	// Vertex
	render->bindVBO(VBO_ARRAY, *vbo);
	render->getAttribLocation(fx, "Vertex", &vertexAttrib);
	render->enableAttribArray(vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, NULL);

	// TexCoord
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
	render->enableAttribArray(texcoordAttrib);
	render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, NULL);

	render->bindVAO(0);
}

void StandardRenderer::initTextVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo, unsigned int fx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	// Update vertex cache
	Vector2 vertices[4];
	vertices[0] = Vector2(0, 0);
	vertices[1] = Vector2(0, 1);
	vertices[3] = Vector2(1, 1);
	vertices[2] = Vector2(1, 0);

	render->createVAO(vao);
	render->bindVAO(*vao);

	render->createVBO(vbo);
	render->bindVBO(VBO_ARRAY, *vbo);
	render->setVBO(VBO_ARRAY, vertices, 4 * sizeof(Vector2), VBO_DYNAMIC);

	render->createVBO(texcoordVbo);
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->setVBO(VBO_ARRAY, m_texCoords, 4 * sizeof(Vector2), VBO_DYNAMIC);

	// Send Vertex data
	int vertexAttrib;
	int texcoordAttrib;

	// Vertex
	render->bindVBO(VBO_ARRAY, *vbo);
	render->getAttribLocation(fx, "Vertex", &vertexAttrib);
	render->enableAttribArray(vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, NULL);

	// TexCoord
	render->bindVBO(VBO_ARRAY, *texcoordVbo);
	render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
	render->enableAttribArray(texcoordAttrib);
	render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, NULL);

	render->bindVAO(0);
}

void StandardRenderer::clearQuadVAO(unsigned int* vao, unsigned int* vbo, unsigned int* texcoordVbo)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	render->deleteVAO(vao);
	render->deleteVBO(vbo);
	render->deleteVBO(texcoordVbo);
}

/// UUUUGLY!
Vector3 g_referenceCameraPos;

bool zCompare(const OEntity* lhs, const OEntity* rhs)
{
	return (lhs->getTransformedPosition() - g_referenceCameraPos).getLength()
		> (rhs->getTransformedPosition() - g_referenceCameraPos).getLength();
}



/**********************************************************************************

2D STUFF

***********************************************************************************/

static const char* colorOnlyVertShader =

"#version 130\n"
"attribute vec3 Vertex;"
"uniform mat4 ProjModelViewMatrix;"
"void main(void)"
"{"
"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
"}\n";

static const char* colorOnlyFragShader = "#version 130\n"
"uniform vec4 color;"
"void main(void)"
"{"
"gl_FragColor = color;"
"}\n";

static const char* texturedVertShader =

"#version 130\n"
"attribute vec3 Vertex;"
"uniform mat4 ProjModelViewMatrix;"

"varying vec2 texCoord;"
"attribute vec2 TexCoord;"

"void main(void)"
"{"
"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
"texCoord = TexCoord;"
"}\n";

static const char* texturedFragShader =
"#version 130\n"
"uniform sampler2D Texture[5];"
"varying vec2 texCoord;"

"void main(void)"
"{"
"gl_FragColor = texture2D(Texture[0], texCoord);"
"}\n";

void StandardRenderer::init2DVao(unsigned int fx, unsigned int* vao)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	Vector2 texCoords[4];
	texCoords[0] = Vector2(0, 1);
	texCoords[1] = Vector2(0, 0);
	texCoords[2] = Vector2(1, 1);
	texCoords[3] = Vector2(1, 0);

	Vector2 vertices[4];
	vertices[0] = Vector2(0, 0);
	vertices[1] = Vector2(0, 1);
	vertices[2] = Vector2(1, 0);
	vertices[3] = Vector2(1, 1);

	render->createVAO(vao);
	render->bindVAO(*vao);

	render->createVBO(&m_vertexVbo);
	render->bindVBO(VBO_ARRAY, m_vertexVbo);
	render->setVBO(VBO_ARRAY, vertices, 4 * sizeof(Vector2), VBO_STATIC);

	render->createVBO(&m_texcoordVbo);
	render->bindVBO(VBO_ARRAY, m_texcoordVbo);
	render->setVBO(VBO_ARRAY, texCoords, 4 * sizeof(Vector2), VBO_DYNAMIC);

	// Send Vertex data
	int vertexAttrib;
	int texcoordAttrib;

	// Vertex
	render->bindVBO(VBO_ARRAY, m_vertexVbo);
	render->getAttribLocation(fx, "Vertex", &vertexAttrib);
	render->enableAttribArray(vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, NULL);

	// TexCoord
	render->bindVBO(VBO_ARRAY, m_texcoordVbo);
	render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
	render->enableAttribArray(texcoordAttrib);
	render->setAttribPointer(texcoordAttrib, VAR_FLOAT, 2, NULL);

	render->bindVAO(0);
}

void StandardRenderer::drawColoredQuad(const Vector2& position, const Vector2& size, const Vector4& color, float rotation)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	// Don't render anything if there is nothing to render
	if (color.w == 0)
		return;

	if (m_colorOnlyFx == 0)
	{
		loadShader(colorOnlyVertShader, colorOnlyFragShader,
			&m_colorOnlyFx);

		init2DVao(m_colorOnlyFx, &m_colorVao);
	}

	int vertexAttrib;
	// render->pushMatrix();

	/*Vector2 m_vertices[4];
	m_vertices[0] = Vector2(x, y);
	m_vertices[1] = Vector2(x, y + h);
	m_vertices[3] = Vector2(x + w, y + h);
	m_vertices[2] = Vector2(x + w, y);*/

	// Set up env
	render->bindVAO(m_colorVao);
	render->bindFX(m_colorOnlyFx);
	
	// projmodelview matrix
	static Matrix4x4 ProjMatrix;
	static Matrix4x4 ModelViewMatrix;
	static Matrix4x4 ProjModelViewMatrix;

	render->getProjectionMatrix(&ProjMatrix);
	render->getModelViewMatrix(&ModelViewMatrix);

	ModelViewMatrix.setScale(Vector3(size.x, size.y, 1));
	ProjMatrix.translate(Vector3(position.x, position.y, 0));

	Vector3 pivot = Vector3(position.x + 0.5 * size.x, position.y + 0.5 * size.y, 0);
	ModelViewMatrix.translate(pivot);
	ModelViewMatrix.rotate(Vector3(0, 0, 1), rotation);
	ModelViewMatrix.translate(-pivot);

	ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;

	render->sendUniformMatrix(m_colorOnlyFx, "ProjModelViewMatrix",
		&ProjModelViewMatrix);

	// Vertex
	/*render->getAttribLocation(m_colorOnlyFx, "Vertex", &vertexAttrib);
	render->setAttribPointer(vertexAttrib, VAR_FLOAT, 2, m_vertices);
	render->enableAttribArray(vertexAttrib);*/

	// Width
	render->sendUniformFloat(m_colorOnlyFx, "Width", (float*) &size.x, 1);
	// Height
	render->sendUniformFloat(m_colorOnlyFx, "Height", (float*) &size.y, 1);

	// Color
	render->sendUniformVec4(m_colorOnlyFx, "color", color);

	// draw
	render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);

	//render->disableAttribArray(vertexAttrib);
	render->bindFX(0);
	render->bindVAO(0);
	// render->popMatrix();
}

void StandardRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture, float rotation)
{
	drawTexturedQuad(position, size, texture, rotation, Vector2(1, 1),
		Vector2(1, 1), Vector4(0.0, 0.0, 1.0, 1.0));
}

void StandardRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
	float rotation, const Vector2& scale)
{
	drawTexturedQuad(position, size, texture, rotation, scale, Vector2(1, 1),
		Vector4(0.0, 0.0, 1.0, 1.0));
}

void StandardRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
	float rotation, const Vector2& scale, const Vector2& flip)
{
	drawTexturedQuad(position, size, texture, rotation, scale, flip,
		Vector4(0.0, 0.0, 1.0, 1.0));
}

void StandardRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
	float rotation, const Vector2& scale, const Vector2& flip, const Vector4& texcoords)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	if (m_texturedFx == 0)
	{
		loadShader(texturedVertShader, texturedFragShader, &m_texturedFx);
		init2DVao(m_texturedFx, &m_textureVao);
	}

	Vector2 m_texcoords[4];
	m_texcoords[0] = Vector2(texcoords.x, texcoords.y + texcoords.w); // (0, 1)
	m_texcoords[1] = Vector2(texcoords.x, texcoords.y); // (0, 0)
	m_texcoords[2] = Vector2(texcoords.x + texcoords.z, texcoords.y + texcoords.w);
	m_texcoords[3] = Vector2(texcoords.x + texcoords.z, texcoords.y);

	// Set up env
	render->bindVAO(m_textureVao);
	render->bindFX(m_texturedFx);
	render->bindTexture(texture);

	// projmodelview matrix
	static Matrix4x4 ProjMatrix;
	static Matrix4x4 ModelViewMatrix;
	static Matrix4x4 ProjModelViewMatrix;
	render->getProjectionMatrix(&ProjMatrix);
	render->getModelViewMatrix(&ModelViewMatrix);

	ModelViewMatrix.setScale(Vector3(size.x, size.y, 1));

	Vector3 pivot = Vector3(position.x + 0.5 * size.x, position.y + 0.5 * size.y, 0.0);

	ProjMatrix.translate(pivot);

	ProjMatrix.rotate(Vector3(1, 0, 0), flip.x);
	ProjMatrix.rotate(Vector3(0, 1, 0), flip.y);
	ProjMatrix.rotate(Vector3(0, 0, 1), rotation);
	ProjMatrix.translate(-pivot);

	ProjMatrix.translate(Vector3(position.x, position.y, 0));
	ModelViewMatrix.scale(Vector3(scale.x, scale.y, 0));

	ProjModelViewMatrix = ProjMatrix * ModelViewMatrix;
	render->sendUniformMatrix(m_texturedFx, "ProjModelViewMatrix",
		&ProjModelViewMatrix);

	// Texcoords
	render->bindVBO(VBO_ARRAY, m_texcoordVbo);
	render->setVBO(VBO_ARRAY, m_texcoords, 4 * sizeof(Vector2), VBO_DYNAMIC);

	// Width
	render->sendUniformFloat(m_texturedFx, "Width", (float*) &size.x, 1);
	// Height
	render->sendUniformFloat(m_texturedFx, "Height", (float*) &size.y, 1);

	// draw
	render->drawArray(PRIMITIVE_TRIANGLE_STRIP, 0, 4);

	render->bindFX(0);
	render->bindTexture(0);
	render->bindVAO(0);
}

void StandardRenderer::set2D(const Vector2& size)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	render->setViewport(0, 0, size.x, size.y);

	// set ortho projection
	render->setMatrixMode(MATRIX_PROJECTION);
	render->loadIdentity();

	render->setOrthoView(0.0f, size.x, size.y, 0.0f, 1.0f, -1.0f);

	render->setMatrixMode(MATRIX_MODELVIEW);
	render->loadIdentity();

	render->enableBlending();
	render->setBlendingMode(BLENDING_ALPHA);

	render->disableDepthTest();
	render->disableCullFace();
}

void StandardRenderer::loadShader(const char* vert, const char* frag, unsigned int* fx)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();

	unsigned int vertShad, pixShad;
	render->createVertexShader(&vertShad);
	bool success = render->sendShaderSource(vertShad, vert);

	render->createPixelShader(&pixShad);
	success &= render->sendShaderSource(pixShad, frag);

	if (!success)
	{
		*fx = 0;
	}
	else
	{
		render->createFX(fx, vertShad, pixShad);
		render->deleteShader(&vertShad, *fx);
		render->deleteShader(&pixShad, *fx);
	}
}

void StandardRenderer::drawText2D(OText* text, float x, float y, float rotation)
{
	RenderingContext* renderContext =
		NeoEngine::getInstance()->getRenderingContext();

	text->setPosition(Vector3(x, y, 0));
	text->setRotation(Quaternion(0, 0, rotation));
	text->updateMatrix();

	renderContext->pushMatrix();
	renderContext->multMatrix(text->getMatrix());

	renderContext->enableTexture();

	drawText(text);
	renderContext->disableTexture();
	renderContext->popMatrix();
}

void StandardRenderer::sendTexture(unsigned int id, Image* image, bool mipMap, bool filter, bool compress)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	render->bindTexture(id);

	render->sendTextureImage(image, mipMap, filter, compress);
	render->bindTexture(0);
}

unsigned int StandardRenderer::createTexture()
{
	unsigned int id;
	NeoEngine::getInstance()->getRenderingContext()->createTexture(&id);
	return id;
}

void StandardRenderer::destroyTexture(unsigned int id)
{
	NeoEngine::getInstance()->getRenderingContext()->deleteTexture(&id);
}

void StandardRenderer::clearScreen(const Vector3& color)
{
	auto render = NeoEngine::getInstance()->getRenderingContext();
	render->setClearColor(color);
	render->clear(BUFFER_COLOR | BUFFER_DEPTH);
}

/*OText* StandardRenderer::create2DText(const char* font, float size)
{
	if (!font)
		return NULL;

	SystemContext* system = NeoEngine::getInstance()->getSystemContext();

	char file[256];
	getGlobalFilename(file, system->getWorkingDirectory(), font);

	OText* text;
	text = new OText(NeoEngine::getInstance()->getLevel()->loadFont(file));
	text->setSize(size);

	return text;
}*/
