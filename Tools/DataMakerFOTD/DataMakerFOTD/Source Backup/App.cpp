#include "App.h"

#include "MainScreen.h"

App::App()
{
    m_window.setScreenTitle("Data Maker for FOTD");
    m_window.setScreenSize(1028, 624); // Alex: 1920x1080
}

App::~App()
{
    //dtor
}

void App::onInit() {

}

void App::addScreens() {
    m_mainScreen = std::make_unique<MainScreen>(&m_window);

    m_screenList->addScreen(m_mainScreen.get());

    m_screenList->setScreen(m_mainScreen->getScreenIndex());
}

void App::onExit() {

}
