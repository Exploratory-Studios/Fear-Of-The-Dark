#pragma once

#include "StartupScreen.h"
#include "MainMenuScreen.h"
#include "OptionsMenuScreen.h"
#include "LoadScreen.h"
#include "GameplayScreen.h"

#include "WorldManager.h"

#include "ScreenIndices.h"

#include <IMainGame.h>
#include <ScreenList.h>

class App : public GLEngine::IMainGame
{
    public:
        App();
        virtual ~App();
        virtual void onInit() override;
        virtual void addScreens() override;
        virtual void onExit() override;

    private:
        std::unique_ptr<StartupScreen> m_startupScreen;
        std::unique_ptr<MainMenuScreen> m_mainMenuScreen;
        std::unique_ptr<OptionsMenuScreen> m_optionsMenuScreen;
        std::unique_ptr<LoadScreen> m_loadScreen;
        std::unique_ptr<GameplayScreen> m_gameplayScreen;

        WorldManager* m_worldManager = new WorldManager();
};
