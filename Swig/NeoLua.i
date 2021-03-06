%module NeoLua
%{
        #include <NeoEngine.h>
        #include <NeoCore.h>
        #include <Neo2DEngine.h>
        #include <Widget.h>
        #include <Button.h>

        #include <ThemedButton.h>
        #include <Sprite.h>
        #include <Tile.h>
        #include <InputField.h>
        #include <Canvas.h>
        #include <Label.h>

        using namespace Neo;
        using namespace Neo2D;
        using namespace Gui;

        // #define Canvas Neo::Gui::Canvas
%}

%include <windows.i>
%include <cpointer.i>

#define NEO_CORE_EXPORT

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



#define NEO_CORE_EXPORT
#define NEO2D_EXPORT
%include <Neo2D.h>
%include <Widget.h>
%include <Canvas.h>
%include <Neo2DEngine.h>
%include <Button.h>

%include <Sprite.h>
%include <Tile.h>
%include <InputField.h>
%include <ThemedButton.h>
%include <Label.h>


//  Some pointer stuff
%pointer_class(int, intp);
%pointer_class(unsigned int, uintp);
%pointer_class(float, floatp);
%pointer_class(double, doublep);
%pointer_class(Neo::Vector3, Vector3p);


