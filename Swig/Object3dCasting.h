#ifndef OBJECT3D_CASTING_H
#define OBJECT3D_CASTING_H

#include <NeoEngine.h>

namespace Neo
{
OLight* castToLight(Object3d* obj) { return dynamic_cast<OLight*>(obj); }
OEntity* castToEntity(Object3d* obj) { return dynamic_cast<OEntity*>(obj); }
OText* castToText(Object3d* obj) { return dynamic_cast<OText*>(obj); }
OSound* castToSound(Object3d* obj) { return dynamic_cast<OSound*>(obj); }
OCamera* castToCamera(Object3d* obj) { return dynamic_cast<OCamera*>(obj); }

OLight* castToLight(OLight* obj) { return obj; }
OEntity* castToEntity(OEntity* obj) { return obj; }
OText* castToText(OText* obj) { return obj; }
OSound* castToSound(OSound* obj) { return obj; }
OCamera* castToCamera(OCamera* obj) { return obj; }
}

#endif