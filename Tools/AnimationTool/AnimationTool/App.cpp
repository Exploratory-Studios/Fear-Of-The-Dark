#include "App.h"

App::App() {
	m_window.setScreenTitle("Fear Of The Dark - Animation Tool");
	m_window.setScreenSize(1024, 624);
}

App::~App() {

}

void App::onInit() {

}

void App::addScreens() {
	m_mainScreen = std::make_unique<MainScreen>(&m_window);

	/// MAKE SURE YOU ADD SCREENS IN THE ORDER OF THEIR INDICES

	m_screenList->addScreen(m_mainScreen.get());

	m_screenList->setScreen(m_mainScreen->getScreenIndex());

}

void App::onExit() {

}
