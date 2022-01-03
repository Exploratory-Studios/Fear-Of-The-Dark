#pragma once

#include "ScreenIndices.h"
#include "PresetValues.h"

#include <Screen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BasicRenderer.hpp>
#include <FontRenderer.hpp>
#include <Vertex.hpp>
#include <ResourceManager.hpp>

void addBackgroundImage(std::vector<BARE2D::Texture>& backgroundImages, std::string filename);

enum class NextScreen { OPTIONS, PLAY, EXIT };

class MainMenuScreen : public BARE2D::Screen {
  public:
	MainMenuScreen(BARE2D::Window* window);
	virtual ~MainMenuScreen();

	virtual int	 getNextScreenIndex() const override;
	virtual void initScreen() override;
	virtual void destroyScreen() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update(double dt) override;
	virtual void draw() override;

  private:
	void initUI();

	bool EventPlayButtonClicked(const CEGUI::EventArgs& e);
	bool EventOptionsButtonClicked(const CEGUI::EventArgs& e);
	bool EventExitButtonClicked(const CEGUI::EventArgs& e);

	CEGUI::PushButton*	  m_playButton	  = nullptr;
	CEGUI::PushButton*	  m_optionsButton = nullptr;
	CEGUI::PushButton*	  m_exitButton	  = nullptr;
	CEGUI::DefaultWindow* m_titleLine1	  = nullptr;
	CEGUI::DefaultWindow* m_titleLine2	  = nullptr;

	BARE2D::Camera2D	   m_uiCamera;
	BARE2D::Window*		   m_window		  = nullptr;
	BARE2D::BasicRenderer* m_renderer	  = nullptr;
	BARE2D::FontRenderer*  m_fontRenderer = nullptr;

	std::vector<BARE2D::Texture> m_backgroundImages;
	BARE2D::Colour				 m_backgroundImageColour = BARE2D::Colour(255, 255, 255, 50);
	int							 m_currentBackgroundImage;

	float m_time				 = 0.0f;
	float m_backgroundChangeTime = 15.0f;

	int m_nextScreenIndex = SCREEN_INDEX_MAINMENU;

	bool m_inited = false;
};
