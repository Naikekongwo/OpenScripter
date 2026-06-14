#include "OpenScripter/OpenScripter.hpp"
#include "OpenCore.hpp"
#include "OpenScripter/Stage/PreloadStage.hpp"
#include <exception>
#include <memory>

void OpenScripter::StartUp()
{
    // 初始化OpenScripter
    (void)OpenEngine::getInstance();

    auto &OCore = OpenEngine::getInstance();

    if (!OCore.Initialize())
    {
        LOG("OpenCore 引擎在初始化时出现问题");
        return;
    }

    auto timer = OCore.getTimer();
    auto sCon = OCore.getStageController();

    auto preloadStage = std::make_unique<PreloadStage>(timer, sCon);

    GraphicsInfo gInfo;
    gInfo.nearestScaling = false;
    gInfo.resolutionWidth = 3840;
    gInfo.resolutionHeight = 2160;
    gInfo.targetFrameRate = 144;

    auto OpenScripterInfo = std::make_unique<GameInfo>();
    OpenScripterInfo->_graphicsInfo = gInfo;
    OpenScripterInfo->beta = true;
    OpenScripterInfo->version_major = 0;
    OpenScripterInfo->version_minor = 1;
    OpenScripterInfo->entranceStage = std::move(preloadStage);
    OpenScripterInfo->gameName = "OpenScripter 脚本设计器";

    OCore.GameRegistry(std::move(OpenScripterInfo));

    try
    {
        OCore.MainLoop();
        OCore.CleanUp();
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
        return;
    }
}