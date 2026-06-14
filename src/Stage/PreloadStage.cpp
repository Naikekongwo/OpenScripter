#include "OpenScripter/Stage/PreloadStage.hpp"
#include "OpenCore.hpp"
#include "Runtime/Graphics/Manager/ElementManager.hpp"
#include "World/Stage/StageManager.hpp"

PreloadStage::PreloadStage(Timer *timer, StageManager *sController)
    : OverlayStage(timer, sController, StageType::overlayStage)
{
}

void PreloadStage::onEnter()
{
    LOG("PreloadStage: onEnter - starting resource load");
    LoadingState = OpenCoreManagers::ResManager.LoadResourcesFromJson(10001);
    initializeComponents();
}

void PreloadStage::onExit()
{
    Elements->onDestroy();
    LOG("PreloadStage: onExit - cleared elements");
}

void PreloadStage::onDestroy() { LOG("PreloadStage: onDestroy"); }

bool PreloadStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}

void PreloadStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
    pipeline.onUpdate(timer->getTotalTime());
}

void PreloadStage::onRender() { Elements->onRender(); }

void PreloadStage::initializeComponents()
{
    pipeline
        .next(
            [this]() -> bool
            {
                return LoadingState.wait_for(std::chrono::milliseconds(0)) ==
                       std::future_status::ready;
            })
        .next(
            [this]() -> bool
            {
                auto frameCounter =
                    UI<FrameCounter>("frameCounter", 100, 0, 0, 0);
                frameCounter->Configure().Sequence(true);
                frameCounter->Animate().Timer(6.0f).Commit();
                Elements->PushElement(std::move(frameCounter));
                return true;
            })
        .next(
            [this]() -> bool
            {
                sController->removeStage(StageType::overlayStage);
                return true;
            });
}