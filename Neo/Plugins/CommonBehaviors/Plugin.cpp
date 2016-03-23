#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);
#endif

#include <NeoEngine.h>
#include "FollowBehavior.h"
#include "LookAtBehavior.h"
#include "ParticleSystemBehavior.h"

void StartPlugin()
{
	Neo::NeoEngine* engine = Neo::NeoEngine::getInstance();
	Neo::BehaviorManager* manager = engine->getBehaviorManager();

	manager->addBehavior(Neo::LookAtBehavior::getStaticName(), OBJECT3D_CAMERA, Neo::LookAtBehavior::getNew);
	manager->addBehavior(Neo::FollowBehavior::getStaticName(), OBJECT3D, Neo::FollowBehavior::getNew);
	manager->addBehavior(Neo::ParticleSystemBehavior::getStaticName(), OBJECT3D, Neo::ParticleSystemBehavior::getNew);
}

void EndPlugin()
{

}
