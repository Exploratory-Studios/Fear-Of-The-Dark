#pragma once

#include "StartupScreen.h"
#include "MainMenuScreen.h"
#include "OptionsMenuScreen.h"
#include "LoadScreen.h"
#include "WaitScreen.h"
#include "GameplayScreen.h"

#include "WorldIOManager.h"

#include "ScreenIndices.h"

#include "Options.h"

#include <App.hpp>
#include <ScreenList.hpp>

class App : public BARE2D::App
{
public:
	App();
	~App();

	void addScreens();

private:
	void initXML();

	StartupScreen* m_startupScreen = nullptr;
	MainMenuScreen*	m_mainMenuScreen = nullptr;
	OptionsMenuScreen* m_optionsMenuScreen = nullptr;
	LoadScreen* m_loadScreen = nullptr;
	WaitScreen*	m_waitScreen = nullptr;
	GameplayScreen* m_gameplayScreen = nullptr;

	WorldIOManager* m_WorldIOManager = new WorldIOManager();
	Options*		m_options		 = new Options;
};
