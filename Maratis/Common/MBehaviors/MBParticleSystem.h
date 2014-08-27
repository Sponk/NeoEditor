/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MBParticleSystem.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
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


#ifndef _MB_PARTICLE_SYSTEM_H
#define _MB_PARTICLE_SYSTEM_H

#include <MSDLThread/MSDLThread.h>

class MBParticleSystem : public MBehavior
{
public:

	// constructors / destructors
    MBParticleSystem(MObject3d * parentObject);
    MBParticleSystem(MBParticleSystem & behavior, MObject3d * parentObject);
    ~MBParticleSystem(void);

private:

    struct Particle
    {
        MVector3 position;
        MVector3 speed;
        float size;
        float alpha;
        float spin;
        int time;
        bool alive;
    };

	// variables
    bool m_looping;
    bool m_emitting;
    float m_lifeTime;
    float m_particlesNumber;
    MVector3 m_initialSpeed;
    MVector3 m_gravity;
    float m_speedDivergence;
    float m_lifeDivergence;
    float m_size;
    float m_speedMultiplier;

    float m_alpha;
    float m_alphaDivergence;

    float m_sizeDivergence;
    float m_emissionDelay;

    bool m_multithreading;

    float m_emissionTimer;

    float m_oldParticlesNumber;

    MString m_textureFile;
    MString m_currentTextureFile;
    MTextureRef* m_texRef;

    vector<Particle> m_particles;
    MVector3* m_particlePositions;

    // Attention: Is not used to convey color in any way!
    // Instead it contains additional data like spin or size
    MVector4* m_particleColors;

    unsigned int m_fx;
    unsigned int m_vertShad;
    unsigned int m_pixShad;

    MSDLThread m_thread;
    MSDLSemaphore m_semaphore;

    void updateParticles(MVector3 parentPosition);
    void updateArrays(bool updateColorData);
    inline void applySpeed();

    static int thread_main(void* particlesystem);

public:

	// destroy
	void destroy(void);

	// get new
	static MBehavior * getNew(MObject3d * parentObject);

	// get copy
	MBehavior * getCopy(MObject3d * parentObject);

	// name
    static const char * getStaticName(void){ return "ParticleSystem"; }
	const char * getName(void){ return getStaticName(); }

	// events
	void update(void);
	void runEvent(int param){}
    void draw();

	// variables
	unsigned int getVariablesNumber(void);
	MVariable getVariable(unsigned int id);
};

#endif
