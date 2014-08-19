/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBParticleSystem.h
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
#include "MBParticleSystem.h"
#include <string>

// Shader
std::string particleVertShader =

//"#version 140\n"
"attribute vec3 Vertex;"

"void main(void)"
"{"
    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"}\n";

std::string particleFragShader =
"#version 140\n"
"uniform sampler2D Texture[5];"

"void main(void)"
"{"
    "gl_FragColor = texture2D(Texture[0], gl_PointCoord);"
"}\n";

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MBParticleSystem::MBParticleSystem(MObject3d * parentObject):
MBehavior(parentObject),
m_lifeTime(2000),
m_particlePositions(NULL),
m_speedDivergence(0.0f),
m_lifeDivergence(0),
m_size(1.0),
m_textureFile(""),
m_texRef(NULL),
m_fx(0),
m_speedMultiplier(1.0)
{}

MBParticleSystem::MBParticleSystem(MBParticleSystem & behavior, MObject3d * parentObject):
MBehavior(parentObject),
m_lifeTime(behavior.m_lifeTime),
m_speedDivergence(behavior.m_speedDivergence),
m_lifeDivergence(behavior.m_lifeDivergence),
m_size(behavior.m_size),
m_particlePositions(NULL),
m_textureFile(""),
m_texRef(NULL),
m_fx(0),
m_speedMultiplier(behavior.m_speedMultiplier)
{}

MBParticleSystem::~MBParticleSystem(void)
{
    SAFE_DELETE(m_particlePositions);
}

void MBParticleSystem::destroy(void)
{
	delete this;
}

MBehavior * MBParticleSystem::getNew(MObject3d * parentObject)
{
    return new MBParticleSystem(parentObject);
}

MBehavior * MBParticleSystem::getCopy(MObject3d * parentObject)
{
    return new MBParticleSystem(*this, parentObject);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int MBParticleSystem::getVariablesNumber(void){
    return 9;
}

MVariable MBParticleSystem::getVariable(unsigned int id)
{
	switch(id)
	{
    case 0:
        return MVariable("LifeTime", &m_lifeTime, M_VARIABLE_FLOAT);
        break;
    case 1:
        return MVariable("ParticlesNumber", &m_particlesNumber, M_VARIABLE_FLOAT);
        break;
    case 2:
        return MVariable("InitialSpeed", &m_initialSpeed, M_VARIABLE_VEC3);
        break;
    case 3:
        return MVariable("Gravity", &m_gravity, M_VARIABLE_VEC3);
        break;
    case 4:
        return MVariable("SpeedDivergence", &m_speedDivergence, M_VARIABLE_FLOAT);
        break;
    case 5:
        return MVariable("LifeDivergence", &m_lifeDivergence, M_VARIABLE_FLOAT);
        break;
    case 6:
        return MVariable("Size", &m_size, M_VARIABLE_FLOAT);
        break;
    case 7:
        return MVariable("Texture", &m_textureFile, M_VARIABLE_STRING);
        break;
    case 8:
        return MVariable("SpeedMultiplier", &m_speedMultiplier, M_VARIABLE_FLOAT);
        break;
	default:
		return MVariable("NULL", NULL, M_VARIABLE_NULL);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Events
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void MBParticleSystem::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MGame * game = engine->getGame();
	MLevel * level = engine->getLevel();
	MScene * scene = level->getCurrentScene();

    if(!game->isRunning())
        return;

	MObject3d * parent = getParentObject();

    updateParticles(parent->getTransformedPosition());
}

void MBParticleSystem::draw()
{
    MEngine* engine = MEngine::getInstance();
    MRenderingContext* render = engine->getRenderingContext();
    MGame * game = engine->getGame();

    if(!game->isRunning() || m_particlePositions == NULL)
        return;

    if(m_texRef == NULL)
    {
        char string[256];
        getGlobalFilename(string, engine->getSystemContext()->getWorkingDirectory(), m_textureFile.getSafeString());

        m_texRef = engine->getLevel()->loadTexture(string);
    }

    if(m_fx == 0)
    {
        render->createVertexShader(&m_vertShad);
        render->sendShaderSource(m_vertShad, particleVertShader.c_str());

        render->createPixelShader(&m_pixShad);
        render->sendShaderSource(m_pixShad, particleFragShader.c_str());

        render->createFX(&m_fx, m_vertShad, m_pixShad);
    }

    render->setDepthMask(false);

    if(m_texRef && m_texRef->getTextureId() > 0)
    {
        render->enableTexture();
        //render->setBlendingMode(M_BLENDING_ADD);
        render->enableBlending();
        render->setBlendingMode(M_BLENDING_ALPHA);
        m_texRef->update();
    }

    render->bindFX(m_fx);
    int texIds[4] = { 0, 1, 2, 3 };
    render->sendUniformInt(m_fx, "Textures", texIds, 4);

    render->setPointSize(m_size);
    //render->set
    //render->disableDepthTest();
    //render->disableBlending();
    //render->disableTexture();
    render->enableVertexArray();
    render->setVertexPointer(M_FLOAT, 3, m_particlePositions);

    int vertexAttrib;
    render->getAttribLocation(m_fx, "Vertex", &vertexAttrib);
    render->setAttribPointer(vertexAttrib, M_FLOAT, 3, m_particlePositions);
    render->enableAttribArray(vertexAttrib);

    //render->enableColorArray();
    //render->setColorPointer(M_BYTE, 4, m_Colours);

    render->drawArray(M_PRIMITIVE_POINTS, 0, m_particlesNumber);
    render->setDepthMask(true);
    render->disableAttribArray(vertexAttrib);
}

#define RANDOM 0.5 - 2.0*static_cast<float>(rand()) / static_cast<float>(RAND_MAX)

void MBParticleSystem::updateParticles(MVector3 parentPosition)
{
    MEngine* engine = MEngine::getInstance();
    MSystemContext* system = engine->getSystemContext();

    unsigned long currentTime = system->getSystemTick();
    Particle* particle;

    // Mark dead particles for deletion
    for(int i = 0; i < m_particles.size(); i++)
    {
        particle = &m_particles[i];
        if(particle->time <= currentTime)
            particle->alive = false;
    }

    // Delete dead particles
    for(int i = 0; i < m_particles.size(); i++)
    {
        particle = &m_particles[i];
        if(!particle->alive)
        {
            m_particles.erase(m_particles.begin() + i);
        }
    }

    // Create missing particles
    Particle newParticle;
    MVector3 divergence;

    for(int i = m_particles.size(); i < m_particlesNumber; i++)
    {
        divergence.x = RANDOM * m_speedDivergence;
        divergence.y = RANDOM * m_speedDivergence;
        divergence.z = RANDOM * m_speedDivergence;

        newParticle.alive = true;
        newParticle.time = currentTime + m_lifeTime + RANDOM * m_lifeDivergence;
        newParticle.speed = m_initialSpeed + divergence;
        m_particles.push_back(newParticle);
    }

    // Update existing positions
    for(int i = 0; i < m_particles.size(); i++)
    {
        particle = &m_particles[i];
        particle->position += particle->speed * m_speedMultiplier;
        particle->speed += m_gravity;
    }

    // Update vertex buffer
    if(m_particlePositions == NULL)
    {
        m_particlePositions = new MVector3[static_cast<int>(m_particlesNumber)];
    }

    for(int i = 0; i < m_particles.size(); i++)
    {
        m_particlePositions[i] = m_particles[i].position + parentPosition;
    }
}
