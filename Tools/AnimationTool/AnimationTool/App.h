#pragma once

#include "MainScreen.h"

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
		std::unique_ptr<MainScreen> m_mainScreen;
};
