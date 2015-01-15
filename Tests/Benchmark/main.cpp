#include <NeoEngine.h>
#include <MCore.h>
#include <LuaBehavior.h>
#include <WinContext.h>
#include <PackageManagerNPK.h>
#include <GuiSystem.h>
#include <Widget.h>
#include <Window/Window.h>

using namespace Neo;

void guiSystemGetWidget(unsigned int iterations, unsigned int num_widgets)
{
    MLOG_INFO("Running guiSystemGetWidget benchmark with " << iterations << " iterations and " << num_widgets << " widgets...");
    Gui::GuiSystem* gui = Gui::GuiSystem::getInstance();

    for(int i = 0; i < num_widgets; i++)
    {
        gui->addWidget(NULL);
    }

    unsigned long start = NeoEngine::getInstance()->getSystemContext()->getSystemTick();
    unsigned long end = 0;

    Gui::Widget* w;
    for(int i = 0; i < iterations; i++)
    {
        w = gui->getWidget(i);
    }

    end = NeoEngine::getInstance()->getSystemContext()->getSystemTick();
    start = end - start;

    MLOG_INFO("The test took " << start << " ms. That are " << (float) start/iterations << " ms per iteration.");
}

int main(int argc, char* argv[])
{
    NeoEngine* engine = NeoEngine::getInstance();
    NeoGame* game = new NeoGame();
    Neo::LuaScript* script = new Neo::LuaScript();
    Level* level = new Level();
    MSystemContext* context = (MSystemContext*) new Neo::MWinContext();
    MPackageManager* pmanager = new Neo::MPackageManagerNPK;

    pmanager->init();

    level->addNewScene();
    level->setCurrentSceneId(0);

    engine->setLevel(level);
    engine->setScriptContext(script);
    engine->setSystemContext(context);
    engine->setGame(game);
    engine->setPackageManager(pmanager);

    engine->getBehaviorManager()->addBehavior(Neo::LuaBehavior::getStaticName(), M_OBJECT3D, Neo::LuaBehavior::getNew);

    game->begin();

    MLOG_INFO("Starting Neo benchmarks");

    guiSystemGetWidget(100000, 100000);
}
