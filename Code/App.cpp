#include "App.h"

#include "Singletons.h"

App::App()
{
	getWindow()->setTitle("Fear Of The Dark");
	getWindow()->setSize(1024, 624);

	addScreens();
}

App::~App()
{
}

void App::addScreens()
{
	Singletons::createWorld();

	m_startupScreen		= std::make_unique<StartupScreen>(getWindow(), getInputManager()); /// 0
	m_mainMenuScreen	= std::make_unique<MainMenuScreen>(getWindow());				   /// 1
	m_optionsMenuScreen = std::make_unique<OptionsMenuScreen>(getWindow());				   /// 2
	m_loadScreen		= std::make_unique<LoadScreen>(getWindow(), m_WorldIOManager);	   /// 3
	m_waitScreen		= std::make_unique<WaitScreen>(getWindow(), m_WorldIOManager);	   /// 4
	m_gameplayScreen	= std::make_unique<GameplayScreen>(getWindow(), m_WorldIOManager, getInputManager()); /// 5

	/// MAKE SURE YOU ADD SCREENS IN THE ORDER OF THEIR INDICES

	getScreenList()->addEntryScreen(m_startupScreen.get()); /// 0
	getScreenList()->addScreen(m_mainMenuScreen.get());		/// 1
	getScreenList()->addScreen(m_optionsMenuScreen.get());	/// 2
	getScreenList()->addScreen(m_loadScreen.get());			/// 3
	getScreenList()->addScreen(m_waitScreen.get());			/// 4
	getScreenList()->addScreen(m_gameplayScreen.get());		/// 5
}
