#pragma once

#include <Screen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BasicRenderer.hpp>
#include <FontRenderer.hpp>
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

class LoadScreen : public BARE2D::Screen {
  public:
	LoadScreen(BARE2D::Window* window, WorldIOManager* WorldIOManager);
	virtual ~LoadScreen();

	virtual int	 getNextScreenIndex() const override;
	virtual void initScreen() override;
	virtual void destroyScreen() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update(double dt) override;
	virtual void draw() override;

  private:
	void checkInput();
	void initUI();

	bool onNewWorldCreateNewButtonClicked(const CEGUI::EventArgs& e);
	bool onNewWorldSizeValueChanged(const CEGUI::EventArgs& e);
	bool onNewWorldSizeSliderReleased(const CEGUI::EventArgs& e);
	bool onCreateButtonClicked(const CEGUI::EventArgs& e);
	bool onLoadButtonClicked(const CEGUI::EventArgs& e);
	bool onLoadWorldLoadButtonClicked(const CEGUI::EventArgs& e);
	bool onBackButtonClicked(const CEGUI::EventArgs& e);

	void getDirectoryEntries();

	int m_nextScreenIndex = SCREEN_INDEX_LOAD;

	std::vector<MiniScreenEntry> m_miniScreenEntries;
	MiniScreen					 m_miniScreen = MiniScreen::MAIN;

	float m_time = 0;

	CEGUI::PushButton*	  m_createButton			= nullptr;
	CEGUI::PushButton*	  m_newWorldCreateNewButton = nullptr;
	CEGUI::FrameWindow*	  m_newWorldFramewindow		= nullptr; /// World creation buttons
	CEGUI::Editbox*		  m_newWorldNameEditbox		= nullptr;
	CEGUI::Editbox*		  m_newWorldSeedEditbox		= nullptr;
	CEGUI::ToggleButton*  m_newWorldFlatCheckbox	= nullptr; // To create a flat world
	CEGUI::Slider*		  m_newWorldSizeSlider		= nullptr;
	CEGUI::DefaultWindow* m_newWorldSizeLabel		= nullptr; // For the # of chunks

	CEGUI::PushButton* m_backButton = nullptr;

	CEGUI::PushButton* m_loadButton			  = nullptr;
	CEGUI::PushButton* m_loadWorldLoadButton  = nullptr;
	CEGUI::Listbox*	   m_loadWorldNameListbox = nullptr;

	BARE2D::Window*		   m_window		  = nullptr;
	BARE2D::BasicRenderer* m_renderer	  = nullptr;
	BARE2D::FontRenderer*  m_fontRenderer = nullptr;

	WorldIOManager* m_worldIOManager = nullptr;
};
