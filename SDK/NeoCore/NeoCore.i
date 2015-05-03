%module NeoCore
%{
	#include <NeoCore.h>
	using namespace Neo;
%}

%include <windows.i>

%include <NeoCore.h>
%include <Utils.h>

%include <Maths.h>
%include <Vector2.h>
%include <Vector3.h>
%include <Vector4.h>
%include <Color.h>
%include <Matrix4x4.h>
%include <Quaternion.h>


%include <SystemContext.h>
%include <InputContext.h>
%include <RenderingContext.h>
%include <SoundContext.h>
%include <PhysicsContext.h>
%include <ScriptContext.h>
%include <Image.h>
%include <Sound.h>
%include <StringTools.h>
%include <DataManager.h>
%include <DataLoader.h>
