#include <NeoEngine.h>
#include <Neo2D.h>
#include <Canvas.h>

#ifdef _WIN32
	#include <windows.h>
	extern "C" __declspec(dllexport) void StartPlugin(void);
	extern "C" __declspec(dllexport) void EndPlugin(void);

	__declspec(dllexport) Neo2D::Canvas MainCanvas;
#else
	extern "C" void StartPlugin(void);
	extern "C" void EndPlugin(void);

	Neo2D::Canvas MainCanvas;
#endif

class Neo2DGame : public Neo::SubGame
{
public:
	void update()
	{
		MainCanvas.update(Neo::NeoEngine::getInstance()->getGame()->getFrameDelta());
	}

	void draw()
	{
		MainCanvas.draw();
	}

	void onBegin() {}
	void onEnd() {}
} g_neo2dGame;

void StartPlugin()
{
	Neo::NeoEngine::getInstance()->getGame()->registerSubGame(&g_neo2dGame);
}

void EndPlugin()
{

}
