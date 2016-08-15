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

#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

#include <NeoEngine.h>
#include "GLESRenderer.h"
#include <string>
#include <algorithm>
#include <tinyutf8.h>
#include <atomic>
#include <stdlib.h>

#ifndef SHADER_PATH
#define SHADER_PATH "./"
#endif

using namespace Neo;

//#define GL_DEBUG
#ifdef GL_DEBUG
void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum
                            severity, GLsizei length, GLchar* message, void* userParam)
{

    MLOG_INFO("--------------------- BEGIN OPENGL DEBUG MESSAGE -------------------");

    int size = strlen(message);
    if(size > 0 && message[size-1] == '\n')
        message[size-1] = '\0';

    stringstream ss;

    ss << "\tType: ";

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR_KHR:
        ss << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR:
        ss << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR:
        ss << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY_KHR:
        ss << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE_KHR:
        ss << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER_KHR:
        ss << "OTHER";
        break;
    }
    ss << endl;

    ss << "\tMessage: "<< message << endl;

    ss << "\tID: " << id << endl;
    ss << "\tSeverity: ";

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW_KHR:
        ss << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM_KHR:
        ss << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH_KHR:
        ss << "HIGH";
        break;
    }
    ss << endl << endl;

    MLOG_INFO(ss.str());
    MLOG_INFO("--------------------- END OPENGL DEBUG MESSAGE ---------------------");
}
#endif

static const char* textFragShader = "#version 100\n"
	"precision mediump float;\n"
	"uniform sampler2D Font;"
	"uniform vec3 Color;"
	"varying vec2 texCoord;"
	"void main(void)"
	"{"
	"gl_FragColor = vec4(1,1,1,1);"
	//"gl_FragColor.rgb = Color;"
	//"gl_FragColor.a = texture2D(Font, texCoord).a;"
	"}\n";

static const char* colorOnlyVertShader =

	"#version 100\n"
		"precision mediump float;\n"
		"attribute vec3 Vertex;"
		"uniform mat4 ProjModelViewMatrix;"
		"void main(void)"
		"{"
		"gl_Position = ProjModelViewMatrix * vec4(Vertex, 1.0);"
		"}\n";

static const char* colorOnlyFragShader = "#version 100\n"
	"precision mediump float;\n"
	"uniform vec4 Color;"
	"void main(void)"
	"{"
	"gl_FragColor = Color;"
	"}\n";

static const char* texturedVertShader =

	"#version 100\n"
	"precision mediump float;\n"

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
 	"#version 100\n"
	"precision mediump float;\n"

	"uniform sampler2D Texture;"
	"varying vec2 texCoord;"

	"void main(void)"
	"{"
	"gl_FragColor = texture2D(Texture, texCoord);\n"
	"}\n";

static GLenum neo2gles(VAR_TYPES type)
{
	switch(type)
	{
		default:
		case VAR_BYTE:
			return GL_BYTE;

		case VAR_UBYTE:
			return GL_UNSIGNED_BYTE;

		case VAR_SHORT:
			return GL_SHORT;

		case VAR_USHORT:
			return GL_UNSIGNED_SHORT;

		case VAR_INT:
			return GL_INT;

		case VAR_UINT:
			return GL_UNSIGNED_INT;

		case VAR_FLOAT:
			return GL_FLOAT;
	}
}

static GLenum neo2gles(PRIMITIVE_TYPES type)
{
	switch(type)
	{
		default:
		case PRIMITIVE_LINES:
			return GL_LINES;

		case PRIMITIVE_LINE_LOOP:
			return GL_LINE_LOOP;

		case PRIMITIVE_LINE_STRIP:
			return GL_LINE_STRIP;

		case PRIMITIVE_TRIANGLES:
			return GL_TRIANGLES;

		case PRIMITIVE_TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;

		case PRIMITIVE_POINTS:
			return GL_POINTS;
	}
}

static GLenum neo2gles(CULL_MODES mode)
{
	switch(mode)
	{
		case CULL_FRONT:
			return GL_FRONT;
		case CULL_BACK:
			return GL_BACK;
		case CULL_FRONT_BACK:
			return GL_FRONT_AND_BACK;

		default: return GL_NONE;
	}
}

static void setBlendingMode(BLENDING_MODES mode)
{
	switch(mode)
	{
		case BLENDING_NONE:
			glBlendFunc(GL_ONE, GL_ZERO);
			break;

		case BLENDING_ALPHA:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BLENDING_ADD:
			glBlendFunc(GL_ONE, GL_ONE);
			break;

		case BLENDING_PRODUCT:
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);
			break;

		case BLENDING_LIGHT:
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			break;

		default:
			break;
	}
}

static void setOrthoView(Matrix4x4& matrix, float left, float right, float bottom, float top, float zNear, float zFar)
{
	if(right == left || top == bottom || zFar == zNear)
	{
		// GL_INVALID_VALUE;
		return;
	}

	float tx = - (right + left)/(right - left);
	float ty = - (top + bottom)/(top - bottom);
	float tz = - (zFar + zNear)/(zFar - zNear);

	matrix = Matrix4x4(
		2.0f/(right-left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f/(top-bottom), 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f/(zFar-zNear), 0.0f,
		tx, ty, tz, 1.0f
	);
}

void GLESRenderer::initialize()
{
	MLOG_INFO("********************************************************************************");
	MLOG_INFO("Renderer:\t\t\t" << glGetString(GL_VERSION) << "\t");
	MLOG_INFO("Vendor:\t\t\t" << glGetString(GL_VENDOR) << "\t");
	MLOG_INFO("GLSL Version:\t\t\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\t");

	int data;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &data);
	MLOG_INFO("Number of textures:\t\t" << data << "\t");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLineWidth(1.0f);
	glClearStencil(0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

#ifdef GL_DEBUG
	glEnable(GL_DEBUG_OUTPUT_KHR);
	glDebugMessageCallbackKHR((GLDEBUGPROCKHR)openglCallbackFunction, NULL);

	GLuint unusedIds = 0;
	glDebugMessageControlKHR(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
#endif

	MLOG_INFO("********************************************************************************");

	// Build quad VAO
	{
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

		unsigned int quadVbo[2];
		glGenBuffers(2, quadVbo);

		glBindBuffer(GL_ARRAY_BUFFER, quadVbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vector2), &vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, quadVbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vector2), &vertices, GL_DYNAMIC_DRAW);

		m_vertexVbo = quadVbo[0];
		m_texcoordVbo = quadVbo[1];
	}

	// Load color only shader
	{
		m_colorShader = createShader(colorOnlyVertShader, colorOnlyFragShader);
		m_texturedShader = createShader(texturedVertShader, texturedFragShader);
		m_textShader = createShader(texturedVertShader, textFragShader);

		std::string shaderpath = SHADER_PATH;
		m_objectShader = loadShader((shaderpath + "/gles.vert").c_str(), (shaderpath + "/gles.frag").c_str());
	}

	if(!m_colorShader || !m_texturedShader || !m_objectShader || !m_textShader)
	{
		exit(-1);
	}

	glUseProgram(m_objectShader);
	m_mvpMatrixUniform = glGetUniformLocation(m_objectShader, "ProjModelViewMatrix");
	m_normalMatrixUniform = glGetUniformLocation(m_objectShader, "NormalMatrix");
	m_textureModeUniform = glGetUniformLocation(m_objectShader, "TextureMode");
	m_diffuseUniform = glGetUniformLocation(m_objectShader, "Diffuse");
	m_opacityUniform = glGetUniformLocation(m_objectShader, "Opacity");
	m_lightsCountUniform = glGetUniformLocation(m_objectShader, "LightsCount");
	m_modelViewMatrixUniform = glGetUniformLocation(m_objectShader, "ViewMatrix");
	m_specularUniform = glGetUniformLocation(m_objectShader, "Specular");
	m_shininessUniform = glGetUniformLocation(m_objectShader, "Shininess");
	m_emitUniform = glGetUniformLocation(m_objectShader, "Emit");
	m_ambientLightUniform = glGetUniformLocation(m_objectShader, "AmbientLight");

	m_colorMvpMatrixUniform = glGetUniformLocation(m_colorShader, "ProjModelViewMatrix");
	m_colorColorUniform = glGetUniformLocation(m_colorShader, "Color");

	m_textureMvpMatrixUniform = glGetUniformLocation(m_texturedShader, "ProjModelViewMatrix");
	m_textureTextureUniform = glGetUniformLocation(m_texturedShader, "Texture");
	
	char str[20];
	for(int i = 0; i < 5; i++) // Always identity map textures to their texture units
	{
		snprintf(str, sizeof(str), "Textures[%d]", i);
		int uniform = glGetUniformLocation(m_objectShader, str);
		if(uniform != -1)
		{
			glUniform1i(uniform, i);
		}
	}
}

void GLESRenderer::destroy()
{

}

static void updateSkinning(Mesh* mesh, Armature* armature)
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

static void prepareEntity(OEntity* e, OCamera* c)
{
	if(e->isInvisible())
	{
		e->setVisible(false);
		return;
	}

	e->updateVisibility(c);
	if(!e->isVisible() || !e->isActive())
		return;

	Mesh* mesh = e->getMesh();
	if (mesh->getArmatureAnim())
	{
		animateArmature(
			mesh->getArmature(),
			mesh->getArmatureAnim(),
			e->getCurrentFrame()
		);
	}
	else if (mesh->getArmature())
	{
		mesh->getArmature()->processBonesLinking();
		mesh->getArmature()->updateBonesSkinMatrix();
	}

	if (mesh->getMaterialsAnim())
		animateMaterials(mesh, mesh->getMaterialsAnim(), e->getCurrentFrame());

	if (mesh->getTexturesAnim())
		animateTextures(mesh, mesh->getTexturesAnim(), e->getCurrentFrame());

	updateSkinning(e->getMesh(), mesh->getArmature());
}

int GLESRenderer::findLights(Neo::Scene* scene, Neo::OEntity* entity, Neo::SubMesh* submesh)
{
	int num = 0;

	Box3d* box = submesh->getBoundingBox();
	Vector3 scale = entity->getTransformedScale();

	float minScale = scale.x;
	minScale = MIN(minScale, scale.y);
	minScale = MIN(minScale, scale.z);
	minScale = 1.0f / minScale;

	for(int i = 0; i < scene->getLightsNumber(); i++)
	{
		OLight* light = scene->getLightByIndex(i);

		if(!light->isActive()
			|| !light->isVisible()
			|| light->getRadius() <= 0.0f)
		{
			continue;
		}

		// light box
		Vector3 lightPos = light->getTransformedPosition();
		Vector3 localPos = entity->getInversePosition(lightPos);

		float localRadius = light->getRadius() * minScale;

		Box3d lightBox(
			Vector3(localPos - localRadius),
			Vector3(localPos + localRadius)
		);

		if(!box->isInCollisionWith(lightBox))
			continue;

		m_activeLights[num++] = light;
	}

	if(num < 8)
		m_activeLights[num] = nullptr;

	return num;
}

void GLESRenderer::sendLight(unsigned int fx, OLight* l, int num, const Matrix4x4& matrix)
{
	char str[255];
	char ending[255];

	sprintf(str, "Lights[%d].", num);

	strcpy(ending, str);
	strcat(ending, "Position");
	Vector4 position = matrix * l->getTransformedPosition();
	glUniform3f(glGetUniformLocation(fx, ending), position.x, position.y, position.z);

	strcpy(ending, str);
	strcat(ending, "Diffuse");
	position = l->getFinalColor();
	glUniform3f(glGetUniformLocation(fx, ending), position.x, position.y, position.z);

	float intensity = l->getIntensity();
	strcpy(ending, str);
	strcat(ending, "Intensity");
	glUniform1f(glGetUniformLocation(fx, ending), intensity);

	float radius = l->getRadius();
	strcpy(ending, str);
	strcat(ending, "Radius");
	glUniform1f(glGetUniformLocation(fx, ending), radius);

	float spotAngle = cosf(l->getSpotAngle() * DEG_TO_RAD);
	strcpy(ending, str);
	strcat(ending, "SpotCos");
	glUniform1f(glGetUniformLocation(fx, ending), spotAngle);

	float spotExponent = l->getSpotExponent();
	strcpy(ending, str);
	strcat(ending, "SpotExp");
	glUniform1f(glGetUniformLocation(fx, ending), spotExponent);

	strcpy(ending, str);
	strcat(ending, "SpotDir");
	position = matrix.getRotatedVector3(l->getRotatedVector(Vector3(0, 0, -1))).getNormalized();
	glUniform3f(glGetUniformLocation(fx, ending), position.x, position.y, position.z);

	float quadraticAttenuation = 0.0;
	// attenuation
	if (l->getSpotAngle() > 0.0f)
	{
		quadraticAttenuation = (8.0f / l->getRadius());
		quadraticAttenuation = (quadraticAttenuation * quadraticAttenuation) * l->getIntensity();
	}

	strcpy(ending, str);
	strcat(ending, "QuadraticAttenuation");
	glUniform1f(glGetUniformLocation(fx, ending), quadraticAttenuation);

	// Constant attenuation = 1
	float attenuation = 1.0;
	strcpy(ending, str);
	strcat(ending, "ConstantAttenuation");
	glUniform1f(glGetUniformLocation(fx, ending), attenuation);

	// Linear attenuation = 0
	attenuation = 0.0;
	strcpy(ending, str);
	strcat(ending, "LinearAttenuation");
	glUniform1f(glGetUniformLocation(fx, ending), attenuation);

	/*if (l->isCastingShadow())
	{

	}*/
}

void GLESRenderer::drawScene(Scene* scene, OCamera* camera)
{
	Vector2 size = NeoEngine::getInstance()->getSystemContext()->getScreenSize();
	glViewport(0, 0, size.x, size.y);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if(scene->getEntitiesNumber() == 0 && scene->getTextsNumber() == 0) return;

	m_matrix.loadIdentity();
	camera->enable();

	glUseProgram(m_objectShader);
	glUniform3f(m_ambientLightUniform,
				scene->getAmbientLight().x*0.25,
				scene->getAmbientLight().y*0.25,
				scene->getAmbientLight().z*0.25); // The color is too bright most of the time

	// Prepare objects
	PROFILE_BEGIN("ScenePreparation")
//#pragma omp parallel for shared(camera) shared(scene)
	for(int i = 0; i < scene->getEntitiesNumber(); i++)
	{
		OEntity* e = scene->getEntityByIndex(i);

		prepareEntity(e, camera);

		if(!e->isVisible() && e->isActive())
			drawEntity(e, scene, camera);
	}
	PROFILE_END("ScenePreparation")

	for(int i = 0; i < scene->getTextsNumber(); i++)
	{
		drawText(scene->getTextByIndex(i), camera);
	}
}

void GLESRenderer::drawText(OText* textObj, OCamera* camera)
{
	Font* font = textObj->getFont();
	const char* text = textObj->getText();
	vector<float>* linesOffset = textObj->getLinesOffset();

	if(!(strlen(text) > 0 && font))
		return;

	if(linesOffset->size() == 0)
		return;

	int vertAttribIndex;
	int texAttribIndex;
	static Vector2 vertices[4];
	static Vector2 texCoords[4];
	static Matrix4x4 ProjModelViewMatrix;

	// Matrix
	if(camera)
	{
		ProjModelViewMatrix = *camera->getCurrentProjMatrix() * (*camera->getCurrentViewMatrix() ** textObj->getMatrix());
	}
	else
	{
		ProjModelViewMatrix = m_matrix;
	}

	// cull face
	glDisable(GL_CULL_FACE);
	glDepthMask(false);
	glEnable(GL_BLEND);
	setBlendingMode(BLENDING_ALPHA);

	// TexCoord
	texAttribIndex = glGetAttribLocation(m_objectShader, "TexCoord");
	if(texAttribIndex != -1)
	{
		glEnableVertexAttribArray(texAttribIndex);
		glVertexAttribPointer(texAttribIndex, 2, GL_FLOAT, false, 0, texCoords);
	}

	// Vertex
	vertAttribIndex = glGetAttribLocation(m_objectShader, "Vertex");
	if(vertAttribIndex != -1)
	{
		glEnableVertexAttribArray(vertAttribIndex);
		glVertexAttribPointer(vertAttribIndex, 2, GL_FLOAT, false, 0, vertices);
	}

	glUseProgram(m_objectShader);

	glUniformMatrix4fv(m_mvpMatrixUniform, 1, false, ProjModelViewMatrix.entries);
	glUniform3f(m_diffuseUniform, textObj->getColor().x, textObj->getColor().y, textObj->getColor().z);
	glUniform1i(m_textureModeUniform, 7);

	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font->getTextureId());

	unsigned int lineId = 0;
	float lineOffset = (*linesOffset)[0];

	float size = textObj->getSize();
	float tabsize = size * 2.0f;
	float fontSize = (float) font->getFontSize();
	float widthFactor = font->getTextureWidth() / fontSize;
	float heightFactor = font->getTextureHeight() / fontSize;

	unsigned int state = 0;
	unsigned int character = 0;
	unsigned char* s = (unsigned char*) text;

	float xpos = 0;
	float ypos = 0;
	int line = 0;

	for(;*s;s++)
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

		Character* c = font->getCharacter(character);
		if (!c)
			continue;

		Vector2 pos = c->getPos();
		Vector2 scale = c->getScale();
		Vector2 offset = c->getOffset() * size;

		float width = scale.x * widthFactor * size;
		float height = scale.y * heightFactor * size;

		// Round to keep the text pixel aligned when doing
		// 2D rendering. Should not affect 3D text much.
		offset.x += floor(lineOffset);

		texCoords[0] = Vector2(pos.x, (pos.y + scale.y));
		vertices[0] = Vector2(xpos, (ypos + height)) + offset;

		texCoords[1] = Vector2((pos.x + scale.x), (pos.y + scale.y));
		vertices[1] = Vector2((xpos + width), (ypos + height)) + offset;

		texCoords[3] = Vector2((pos.x + scale.x), pos.y);
		vertices[3] = Vector2((xpos + width), ypos) + offset;

		texCoords[2] = Vector2(pos.x, pos.y);
		vertices[2] = Vector2(xpos, ypos) + offset;

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		xpos += c->getXAdvance() * size;
	}

	// disable attribs
	if(vertAttribIndex != -1)
		glDisableVertexAttribArray(vertAttribIndex);

	if(texAttribIndex != -1)
		glDisableVertexAttribArray(texAttribIndex);

	glUseProgram(0);
	glDepthMask(true);
}

void GLESRenderer::drawText2D(OText* text, float x, float y, float rotation)
{
	Matrix4x4 backup = m_matrix;

	text->setPosition(Vector3(floor(x), floor(y), 0));
	text->setRotation(Quaternion(0, 0, rotation));
	text->updateMatrix();

	m_matrix *= *text->getMatrix();

	drawText(text, nullptr);

	m_matrix = backup;
}

void GLESRenderer::drawEntity(OEntity* e, OCamera* camera)
{
	drawEntity(e, nullptr, camera);
}

void GLESRenderer::drawEntity(OEntity* e, Scene* scene, OCamera* camera)
{
	Mesh* mesh = e->getMesh();
	int num = mesh->getSubMeshsNumber();
	SubMesh* subMeshes = mesh->getSubMeshs();
	Armature* armature = mesh->getArmature();

	glUseProgram(m_objectShader);

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

		if(scene)
			findLights(scene, e, &subMeshes[i]);

		Matrix4x4 MVP;
		Matrix4x4 MV;

		MV = *camera->getCurrentViewMatrix() * *e->getMatrix();
		MVP = *camera->getCurrentProjMatrix() * MV;

		glUniformMatrix4fv(m_mvpMatrixUniform, 1, false, MVP.entries);
		glUniformMatrix4fv(m_modelViewMatrixUniform, 1, false, MV.entries);
		glUniformMatrix4fv(m_normalMatrixUniform, 1, true, MV.getInverse().entries);

		{
			int i = 0;
			for (OLight** light = m_activeLights; *light != nullptr && i < 8; light++, i++)
				sendLight(m_objectShader, *light, i, camera->getCurrentViewMatrix()->entries);

			glUniform1i(m_lightsCountUniform, i);
		}

		for (int j = 0; j < subMeshes[i].getDisplaysNumber(); j++)
		{
			MaterialDisplay* display = subMeshes[i].getDisplay(j);
			Material* material = e->getMaterial(display->getMaterialId());
			int texturePasses = material->getTexturesPassNumber();

			initVBO(&subMeshes[i]);

			for (int k = 0; k < texturePasses; k++)
			{
				TexturePass* pass = material->getTexturePass(k);

				if (!pass || !pass->getTexture())
				{
					continue;
				}

				TextureRef* tex = pass->getTexture()->getTextureRef();

				glActiveTexture(GL_TEXTURE0 + k);
				glBindTexture(GL_TEXTURE_2D, tex->getTextureId());
			}

			prepareMaterialDisplay(m_objectShader, &subMeshes[i], display);

			if(subMeshes[i].getColorsSize())
				texturePasses = 6; // The sixth texturing mode: A per-vertex color array

			glUniform1i(m_textureModeUniform, texturePasses);
			glUniform1f(m_opacityUniform, material->getOpacity());

			Vector3 vecData = material->getDiffuse();
			glUniform3f(m_diffuseUniform, vecData.x, vecData.y, vecData.z);

			vecData = material->getSpecular();
			glUniform3f(m_specularUniform, vecData.x, vecData.y, vecData.z);

			vecData = material->getEmit();
			glUniform3f(m_emitUniform, vecData.x, vecData.y, vecData.z);

			glUniform1f(m_shininessUniform, material->getShininess());

			glEnable(GL_BLEND);
			if(e->hasTransparency())
			{
				setBlendingMode(material->getBlendMode());
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				setBlendingMode(BLENDING_ALPHA);
				glEnable(GL_DEPTH_TEST);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			}

			if(display->getCullMode() != CULL_NONE)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(neo2gles(display->getCullMode()));
			}

			if(subMeshes[i].getIndices())
			{
				if(*subMeshes[i].getVBOid2() > 0)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *subMeshes[i].getVBOid2());

					switch(subMeshes[i].getIndicesType())
					{
						case VAR_USHORT:
							glDrawElements(neo2gles(display->getPrimitiveType()),
										   display->getSize(),
										   GL_UNSIGNED_SHORT,
										   (void*)(display->getBegin()*sizeof(short)));
							break;
						case VAR_UINT:
							glDrawElements(neo2gles(display->getPrimitiveType()),
										   display->getSize(),
										   GL_UNSIGNED_INT,
										   (void*)(display->getBegin()*sizeof(int)));
							break;
					}
				}
				else
				{
					switch(subMeshes[i].getIndicesType())
					{
						case VAR_USHORT:
							glDrawElements(neo2gles(display->getPrimitiveType()),
										   display->getSize(),
										   GL_UNSIGNED_SHORT,
										   static_cast<unsigned short*>(subMeshes[i].getIndices()) + display->getBegin());
							break;
						case VAR_UINT:
							glDrawElements(neo2gles(display->getPrimitiveType()),
										   display->getSize(),
										   GL_UNSIGNED_INT,
										   static_cast<unsigned int*>(subMeshes[i].getIndices()) + display->getBegin());
							break;
					}
				}
			}
			else
			{
				glDrawArrays(neo2gles(display->getPrimitiveType()), display->getBegin(), display->getSize());
			}
		}
	}

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mesh->updateBoundingBox();
}

void GLESRenderer::prepareMaterialDisplay(unsigned int fx, SubMesh* mesh, MaterialDisplay* display)
{
	RenderingContext* render = NeoEngine::getInstance()->getRenderingContext();
	void* indices = mesh->getIndices();
	Vector2* texcoords = mesh->getTexCoords();

	unsigned int* vbo1 = mesh->getVBOid1();
	unsigned int* vbo2 = mesh->getVBOid2();

	// Bind VBOs
	if (*vbo1 > 0)
		glBindBuffer(GL_ARRAY_BUFFER, *vbo1);

	if (indices) // If the SubMesh has indices
	{
		if (*vbo2 > 0) // If the indices are stored in the VBO
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *vbo2);
		}
	}

	// Set up vertex attribute
	int attrib;
	size_t offset = 0;

	attrib = glGetAttribLocation(fx, "Vertex");
	if (attrib != -1)
	{
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, false, 0, NULL);
	}

	offset = sizeof(Vector3) * (mesh->getVerticesSize() + display->getBegin());

	// Set up normal attribute
	attrib = glGetAttribLocation(fx, "Normal");
	if (attrib != -1)
	{
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, false, 0, (void*) offset);
	}

	offset += sizeof(Vector3) * mesh->getNormalsSize();

	// Set up tangent attribute
	attrib = glGetAttribLocation(fx, "Tangent");
	if (attrib != -1)
	{
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, false, 0, (void*) offset);
	}

	offset += sizeof(Vector3) * mesh->getTangentsSize();

	// Set up color attribute
	attrib = glGetAttribLocation(fx, "Color");
	if (attrib != -1)
	{
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, false, 0, (void*) offset);
	}

	// Set up texcoord attribute
	if (display->getMaterial()->getTexturesPassNumber() > 0
		&& texcoords != NULL)
	{
		// vert + normal + tang
		offset = sizeof(Vector3)
			* (mesh->getVerticesSize() + mesh->getNormalsSize() + mesh->getTangentsSize() + display->getBegin());

		attrib = glGetAttribLocation(fx, "TexCoord");
		if (attrib != -1)
		{
			glEnableVertexAttribArray(attrib);
			glVertexAttribPointer(attrib, 2, GL_FLOAT, false, 0, (void*) offset);
		}
	}
}

void GLESRenderer::initVBO(SubMesh* subMesh)
{
	NeoEngine* engine = NeoEngine::getInstance();
	RenderingContext* render = engine->getRenderingContext();

	unsigned int* vboId1 = subMesh->getVBOid1();
	unsigned int* vboId2 = subMesh->getVBOid2();

	unsigned int mode = GL_STATIC_DRAW;
	if (subMesh->getSkinData() || subMesh->getMorphingData())
		mode = GL_DYNAMIC_DRAW;

	if (*vboId1 == 0 || mode == GL_DYNAMIC_DRAW)
	{
		// data
		Color* colors = subMesh->getColors();

		Vector3* vertices = subMesh->getVertices();
		Vector3* normals = subMesh->getNormals();
		Vector3* tangents = subMesh->getTangents();

		Vector2* texCoords = subMesh->getTexCoords();

		if (mode == GL_DYNAMIC_DRAW)
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
			glGenBuffers(1, vboId1);

		glBindBuffer(GL_ARRAY_BUFFER, *vboId1);
		glBufferData(GL_ARRAY_BUFFER, totalSize, 0, mode);

		unsigned int offset = 0;
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Vector3) * subMesh->getVerticesSize(), vertices);
		offset += sizeof(Vector3) * subMesh->getVerticesSize();

		if (normals)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Vector3) * subMesh->getNormalsSize(), normals);
			offset += sizeof(Vector3) * subMesh->getNormalsSize();
		}

		if (tangents)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Vector3) * subMesh->getTangentsSize(), tangents);
			offset += sizeof(Vector3) * subMesh->getTangentsSize();
		}

		if (texCoords)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Vector2) * subMesh->getTexCoordsSize(), texCoords);
			offset += sizeof(Vector2) * subMesh->getTexCoordsSize();
		}

		if (colors)
		{
			glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Color) * subMesh->getColorsSize(), colors);
			offset += sizeof(Color) * subMesh->getColorsSize();
		}

		// indices VBO
		if (indices)
		{
			unsigned int typeSize = indicesType == VAR_USHORT ? sizeof(short) : sizeof(int);

			if (*vboId2 == 0)
				glGenBuffers(1, vboId2);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *vboId2);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, subMesh->getIndicesSize() * typeSize, indices, mode);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void GLESRenderer::clearScreen(const Vector3& color)
{
	glClearColor(color.x, color.y, color.z, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLESRenderer::set2D(const Vector2& size)
{
	set2D(size.x, size.y);
}

void GLESRenderer::set2D(unsigned int w, unsigned int h)
{
	setOrthoView(m_matrix, 0.0f, w, h, 0.0f, 1.0f, -1.0f);
}

void GLESRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
					  float rotation, const Vector2& scale, const Vector2& flip, const Vector4& texcoords)
{
	unsigned int vertexAttrib;
	unsigned int texcoordAttrib;

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo);
	vertexAttrib = glGetAttribLocation(m_texturedShader, "Vertex");
	glEnableVertexAttribArray(vertexAttrib);
	glVertexAttribPointer(vertexAttrib, 2, GL_FLOAT, false, 0, NULL);

	// Update VBO
	Vector2 tex[4];
	tex[0] = Vector2(texcoords.x, texcoords.y + texcoords.w); // (0, 1)
	tex[1] = Vector2(texcoords.x, texcoords.y); // (0, 0)
	tex[2] = Vector2(texcoords.x + texcoords.z, texcoords.y + texcoords.w);
	tex[3] = Vector2(texcoords.x + texcoords.z, texcoords.y);

	glBindBuffer(GL_ARRAY_BUFFER, m_texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vector2), &tex, GL_DYNAMIC_DRAW);

	// render->getAttribLocation(fx, "TexCoord", &texcoordAttrib);
	texcoordAttrib = glGetAttribLocation(m_texturedShader, "TexCoord");
	glEnableVertexAttribArray(texcoordAttrib);
	glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, false, 0, NULL);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, texture);

	Vector3 pivot = Vector3(position.x + 0.5 * size.x, position.y + 0.5 * size.y, 0.0);

	Matrix4x4 ModelViewMatrix;
	ModelViewMatrix.loadIdentity();

	ModelViewMatrix.translate(pivot);
	ModelViewMatrix.rotate(Vector3(1, 0, 0), flip.x);
	ModelViewMatrix.rotate(Vector3(0, 1, 0), flip.y);
	ModelViewMatrix.rotate(Vector3(0, 0, 1), rotation);
	ModelViewMatrix.translate(-pivot);

	ModelViewMatrix.translate(position);
	ModelViewMatrix.scale(Vector3(size.x, size.y, 1));

	ModelViewMatrix = m_matrix * ModelViewMatrix;

	glUseProgram(m_texturedShader);
	glUniformMatrix4fv(m_textureMvpMatrixUniform, 1, false, ModelViewMatrix.entries);
	glUniform1i(m_textureTextureUniform, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLESRenderer::drawColoredQuad(const Vector2& position, const Vector2& size, const Vector4& color, float rotation)
{
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	Matrix4x4 ModelViewMatrix;
	ModelViewMatrix.loadIdentity();

	Vector3 pivot = Vector3(position.x + 0.5 * size.x, position.y + 0.5 * size.y, 0.0);

	ModelViewMatrix.translate(pivot);
	ModelViewMatrix.rotate(Vector3(0, 0, 1), rotation);
	ModelViewMatrix.translate(-pivot);

	ModelViewMatrix.translate(position);
	ModelViewMatrix.scale(Vector3(size.x, size.y, 1));

	ModelViewMatrix = m_matrix * ModelViewMatrix;

	glUseProgram(m_colorShader);
	glUniformMatrix4fv(m_colorMvpMatrixUniform, 1, false, ModelViewMatrix.entries);
	glUniform4f(m_colorColorUniform, color.x, color.y, color.z, color.w);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo);

	unsigned int vertexAttrib;
	vertexAttrib = glGetAttribLocation(m_colorShader, "Vertex");
	glEnableVertexAttribArray(vertexAttrib);
	glVertexAttribPointer(vertexAttrib, 2, GL_FLOAT, false, 0, NULL);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLESRenderer::sendTexture(unsigned int id, Image* image, bool mipMap, bool filter, bool compress)
{
	unsigned int bytePerPix = image->getComponents();
	unsigned int width  = image->getWidth();
	unsigned int height = image->getHeight();

	glBindTexture(GL_TEXTURE_2D, id);

	int format = (bytePerPix == 4) ? GL_RGBA : GL_RGB;
	if(filter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(mipMap)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if(mipMap)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, neo2gles(image->getDataType()), image->getData());
	if(mipMap)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}

unsigned int GLESRenderer::createTexture()
{
	unsigned int tex;
	glGenTextures(1, &tex);
	return tex;
}

void GLESRenderer::destroyTexture(unsigned int id)
{
	glDeleteTextures(1, &id);
}

void GLESRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture, float rotation)
{
	drawTexturedQuad(position, size, texture, rotation, Vector2(1, 1),
					 Vector2(1, 1), Vector4(0.0, 0.0, 1.0, 1.0));
}

void GLESRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
									float rotation, const Vector2& scale)
{
	drawTexturedQuad(position, size, texture, rotation, scale, Vector2(1, 1),
					 Vector4(0.0, 0.0, 1.0, 1.0));
}

void GLESRenderer::drawTexturedQuad(const Vector2& position, const Vector2& size, int texture,
									float rotation, const Vector2& scale, const Vector2& flip)
{
	drawTexturedQuad(position, size, texture, rotation, scale, flip,
					 Vector4(0.0, 0.0, 1.0, 1.0));
}

static bool reportShaderError(unsigned int shader, const char* name)
{
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE)
	{
		// Error!
		MLOG_ERROR("Could not compile " << name << " shader!");

		int logsize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);

		if(logsize == 0)
			return false;

		int written;
		char* log = new char[logsize];

		glGetShaderInfoLog(shader, logsize, &written, log);
		MLOG_ERROR(log);

		delete[] log;
		return false;
	}

	return true;
}

static bool reportProgramError(unsigned int shader)
{
	int result;
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if(result == GL_FALSE)
	{
		// Error!
		MLOG_ERROR("Could not link  shader!");

		int logsize = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logsize);

		if(logsize == 0)
			return false;

		int written;
		char* log = new char[logsize];

		glGetShaderInfoLog(shader, logsize, &written, log);
		MLOG_ERROR(log);

		delete[] log;
		return false;
	}

	return true;
}

unsigned int GLESRenderer::createShader(const char* vertexSource, const char* fragmentSource)
{
	unsigned int vert, frag;
	// FIXME: Error handling!
	vert = glCreateShader(GL_VERTEX_SHADER);
	frag = glCreateShader(GL_FRAGMENT_SHADER);

	const char* array[1] = {vertexSource};
	glShaderSource(vert, 1, array, NULL);
	glCompileShader(vert);
	if(!reportShaderError(vert, "vertex")) return 0;

	array[0] = fragmentSource;
	glShaderSource(frag, 1, array, NULL);
	glCompileShader(frag);
	if(!reportShaderError(frag, "fragment")) return 0; // FIXME: CLEANUP!

	unsigned int program = glCreateProgram();

	glAttachShader(program, vert);
	glAttachShader(program, frag);

	glLinkProgram(program);

	/*glDetachShader(program, vert);
	glDetachShader(program, frag);

	glDeleteShader(vert);
	glDeleteShader(frag);*/

	if(!reportProgramError(program))
	{
		glDeleteProgram(program);
		program = 0;
	}

	return program;
}

unsigned int GLESRenderer::loadShader(const char* vertPath, const char* fragPath)
{
	// FIXME: Preprocessor!
	char* vert, *frag;

	if(!(vert = readTextFile(vertPath)))
	{
		MLOG_ERROR("Vertex shader " << vertPath << " could not be loaded!");
		return 0;
	}

	if(!(frag = readTextFile(fragPath)))
	{
		MLOG_ERROR("Fragment shader " << fragPath << " could not be loaded!");
		return 0;
	}

	unsigned int p = createShader(vert, frag);

	SAFE_FREE(vert);
	SAFE_FREE(frag);

	return p;
}
