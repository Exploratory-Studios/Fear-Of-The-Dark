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

#include <IMainGame.h>
#include <ScreenList.h>

class App : public GLEngine::IMainGame {
  public:
	App();
	~App();
	virtual void onInit() override;
	virtual void addScreens() override;
	virtual void onExit() override;

  private:
	std::unique_ptr<StartupScreen>	   m_startupScreen;
	std::unique_ptr<MainMenuScreen>	   m_mainMenuScreen;
	std::unique_ptr<OptionsMenuScreen> m_optionsMenuScreen;
	std::unique_ptr<LoadScreen>		   m_loadScreen;
	std::unique_ptr<WaitScreen>		   m_waitScreen;
	std::unique_ptr<GameplayScreen>	   m_gameplayScreen;

	WorldIOManager* m_WorldIOManager = new WorldIOManager();
	Options*		m_options		 = new Options;
};
