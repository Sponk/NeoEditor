%module NeoEngine
%{
	#include <NeoEngine.h>
	using namespace Neo;
%}

%include <windows.i>

#define NEO_CORE_EXPORT

%include <Vector2.h>
%include <Vector3.h>
%include <Vector4.h>
%include <Color.h>
%include <Matrix4x4.h>
%include <Quaternion.h>
%include <InputContext.h>
%include <SystemContext.h>
%include <PhysicsContext.h>

%include <NeoEngine.h>
%include <NeoGame.h>
%include <Object3d.h>
%include <Scene.h>
%include <Level.h>
%include <Material.h>
%include <Mesh.h>
%include <OEntity.h>
%include <OLight.h>
%include <OSound.h>
%include <OText.h>
%include <OCamera.h>
%include <Texture.h>
%include <TextureRef.h>
%include <Material.h>
%include <Box3d.h>
%include <Armature.h>
