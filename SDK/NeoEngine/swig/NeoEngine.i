%{
#include <NeoCore.h>
#include <NeoEngine.h>
#include <Player.h>
using namespace Neo;
%}

%include <windows.i>
%include <cpointer.i>
%include <std_string.i>

#define NEO_CORE_EXPORT
#define NEO_ENGINE_EXPORT

%ignore Neo::Profiler::FunctionProfile;
%ignore Neo::Object3d::AdditionalData;
%ignore Neo::Scene::AdditionalData;

%include <Vector2.h>
%include <Vector3.h>
%include <Vector4.h>
 
%include <NeoCore.h>
%include <Color.h>
%include <DataLoader.h>
%include <DataManager.h>
%include <FileTools.h>
%include <Image.h>
%include <InputContext.h>
%include <InputMapping.h>
%include <Maths.h>
%include <Matrix4x4.h>
%include <NeoString.h>
%include <PhysicsContext.h>
%include <Profiler.h>
%include <Quaternion.h>
%include <ScriptContext.h>
%include <Sound.h>
%include <SoundContext.h>
%include <StringTools.h>
%include <SystemContext.h>
%include <Utils.h>

%include <NeoEngine.h>

%include <Armature.h>
%include <ArmatureAnim.h>
%include <ArmatureAnimRef.h>
%include <Behavior.h>
%include <BehaviorCreator.h>
%include <BehaviorManager.h>
%include <Box3d.h>
%include <Font.h>
%include <FontRef.h>
%include <Frustum.h>
%include <FXManager.h>
%include <Key.h>
%include <Level.h>
%include <Log.h>
%include <Material.h>
%include <MaterialAnim.h>
%include <MaterialsAnim.h>
%include <MaterialsAnimRef.h>
%include <Mesh.h>
%include <MeshRef.h>
%include <MeshTools.h>
%include <MorphingData.h>
%include <NeoGame.h>
%include <Object3d.h>
%include <Object3dAnim.h>
%include <OBone.h>
%include <OCamera.h>
%include <OEntity.h>
%include <OLight.h>
%include <OSound.h>
%include <OText.h>
%include <PackageManager.h>
%include <Player.h>
%include <Plugin.h>
%include <Renderer.h>
%include <RendererCreator.h>
%include <RendererManager.h>
%include <Scene.h>
%include <ShaderRef.h>
%include <SkinData.h>
%include <SoundRef.h>
%include <Texture.h>
%include <TextureAnim.h>
%include <TextureRef.h>
%include <TexturesAnim.h>
%include <TexturesAnimRef.h>

//  Some pointer stuff
%pointer_class(int, intp);
%pointer_class(unsigned int, uintp);
%pointer_class(float, floatp);
%pointer_class(double, doublep);

%pointer_cast(void*, Neo::String*, Voidp2String);
%pointer_cast(void*, float*, Voidp2Float);
//%pointer_functions(float*, floatp)

%pointer_class(Neo::String, Stringp);
%pointer_class(Neo::Vector3, Vector3p);


