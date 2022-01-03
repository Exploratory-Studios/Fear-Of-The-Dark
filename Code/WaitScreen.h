#pragma once

#include <Screen.hpp>
#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BasicRenderer.hpp>
#include <FontRenderer.hpp>

#include "WorldIOManager.h"
#include "ScreenIndices.h"

class WaitScreen : public BARE2D::Screen {
  public:
	WaitScreen(BARE2D::Window* window, WorldIOManager* worldIOManager);
	virtual ~WaitScreen();

	virtual int	 getNextScreenIndex() const override;
	virtual void initScreen() override;
	virtual void destroyScreen() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update(double dt) override;
	virtual void draw() override;

  protected:
	void initShaders();
	void initUI();

	void checkInput();

	BARE2D::BasicRenderer* m_renderer = nullptr;
	BARE2D::FontRenderer*  m_fontRenderer = nullptr;

	WorldIOManager* m_worldIOManager = nullptr;
	BARE2D::Window* m_window		 = nullptr;

	CEGUI::ProgressBar*	  m_progressBar = nullptr;
	CEGUI::DefaultWindow* m_messageBox	= nullptr;

	float m_time = 0.0f;

	int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;
};
