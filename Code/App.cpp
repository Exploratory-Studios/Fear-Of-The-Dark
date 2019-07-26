#include "App.h"

App::App()
{
    m_window.setScreenTitle("Fear Of The Dark");
    m_window.setScreenSize(1024, 624);
}

App::~App()
{
    //dtor
}

void App::onInit() {

}

void App::addScreens() {
    m_startupScreen = std::make_unique<StartupScreen>(&m_window);               /// 0
    m_mainMenuScreen = std::make_unique<MainMenuScreen>(&m_window);             /// 1
    m_optionsMenuScreen = std::make_unique<OptionsMenuScreen>(&m_window);       /// 2
    m_loadScreen = std::make_unique<LoadScreen>(&m_window, m_WorldIOManager);                     /// 3
    m_waitScreen = std::make_unique<WaitScreen>(&m_window, m_WorldIOManager);                                       /// 4
    m_gameplayScreen = std::make_unique<GameplayScreen>(&m_window, m_WorldIOManager);             /// 5

    /// MAKE SURE YOU ADD SCREENS IN THE ORDER OF THEIR INDICES

    m_screenList->addScreen(m_startupScreen.get());                             /// 0
    m_screenList->addScreen(m_mainMenuScreen.get());                            /// 1
    m_screenList->addScreen(m_optionsMenuScreen.get());                         /// 2
    m_screenList->addScreen(m_loadScreen.get());                                /// 3
    m_screenList->addScreen(m_waitScreen.get());                                /// 4
    m_screenList->addScreen(m_gameplayScreen.get());                            /// 5

    m_screenList->setScreen(m_startupScreen->getScreenIndex());

}

void App::onExit() {

}
