#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>

#include "ScreenIndices.h"
#include "WorldIOManager.h"
/*
-Wl,-Bstatic
-lboost_system
-lboost_thread
-llua5.3
*/
namespace fs = boost::filesystem;

enum class MiniScreen { MAIN, SETTINGS, CREATE, LOAD };

class MiniScreenEntry {
  public:
	MiniScreenEntry(CEGUI::Window* widget, MiniScreen miniScreen) : m_widget(widget), m_miniScreen(miniScreen) {
	}
	void checkScreen(MiniScreen currentMiniScreen) {
		if(currentMiniScreen == m_miniScreen) {
			m_widget->enable();
			m_widget->setVisible(true);
		} else {
			m_widget->disable();
			m_widget->setVisible(false);
		}
	}
	void dispose() {
		//delete m_widget; These are handled by my GUI wrapper classes. Should only be destroyed by m_gui.destroy()
	}

  private:
	CEGUI::Window* m_widget;
	MiniScreen	   m_miniScreen;
};

class LoadScreen : public GLEngine::IGameScreen {
  public:
	LoadScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager);
	virtual ~LoadScreen();

	virtual int	 getNextScreenIndex() const override;
	virtual int	 getPreviousScreenIndex() const override;
	virtual void build() override;
	virtual void destroy() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update() override;
	virtual void draw() override;

  private:
	void checkInput();
	void initUI();
	void initShaders();

	bool onNewWorldCreateNewButtonClicked(const CEGUI::EventArgs& e);
	bool onNewWorldSizeValueChanged(const CEGUI::EventArgs& e);
	bool onNewWorldSizeSliderReleased(const CEGUI::EventArgs& e);
	bool onCreateButtonClicked(const CEGUI::EventArgs& e);
	bool onLoadButtonClicked(const CEGUI::EventArgs& e);
	bool onLoadWorldLoadButtonClicked(const CEGUI::EventArgs& e);
	bool onBackButtonClicked(const CEGUI::EventArgs& e);

	void getDirectoryEntries();

	int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;

	std::vector<MiniScreenEntry> m_miniScreenEntries;
	MiniScreen					 m_miniScreen = MiniScreen::MAIN;

	float m_time = 0;

	CEGUI::PushButton*	 	m_createButton			   = nullptr;
	CEGUI::PushButton*	 	m_newWorldCreateNewButton  = nullptr;
	CEGUI::FrameWindow*	 	m_newWorldFramewindow	   = nullptr; /// World creation buttons
	CEGUI::Editbox*		 	m_newWorldNameEditbox	   = nullptr;
	CEGUI::Editbox*		 	m_newWorldSeedEditbox	   = nullptr;
	CEGUI::ToggleButton* 	m_newWorldFlatCheckbox	   = nullptr; // To create a flat world
	CEGUI::Slider*		 	m_newWorldSizeSlider	   = nullptr;
	CEGUI::DefaultWindow*	m_newWorldSizeLabel	   	   = nullptr; // For the # of chunks

	CEGUI::PushButton* m_backButton = nullptr;

	CEGUI::PushButton* m_loadButton			  = nullptr;
	CEGUI::PushButton* m_loadWorldLoadButton  = nullptr;
	CEGUI::Listbox*	   m_loadWorldNameListbox = nullptr;

	GLEngine::Camera2D	  m_camera;
	GLEngine::Camera2D	  m_uiCamera;
	GLEngine::Window*	  m_window;
	GLEngine::GUI		  m_gui;
	GLEngine::SpriteBatch m_spriteBatch;
	GLEngine::GLSLProgram m_textureProgram;
	GLEngine::SpriteFont  m_spriteFont;

	Logger* logger = Logger::getInstance();

	WorldIOManager* m_worldIOManager = nullptr;
};
