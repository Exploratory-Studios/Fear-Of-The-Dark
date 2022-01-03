#pragma once

#include "ScreenIndices.h"

#include "PresetValues.h"

#include "Options.h"

#include <Screen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BasicRenderer.hpp>
#include <FontRenderer.hpp>

class Slider {
	/// A wrapper class for a CEGUI::Slider and CEGUI::Label. Give it a title and a pointer to some data!
  public:
	Slider(float* data, glm::vec4 destRect, std::string name, std::string shownName = "");

	void setFont(std::string font);

  private:
	void init(glm::vec4& destRect, std::string& name, std::string& shownName);

	float* m_data = nullptr;

	CEGUI::Slider*		  m_slider = nullptr;
	CEGUI::DefaultWindow* m_label  = nullptr;
};

class OptionsMenuScreen : public BARE2D::Screen {
  public:
	OptionsMenuScreen(BARE2D::Window* window);
	virtual ~OptionsMenuScreen();

	virtual int	 getNextScreenIndex() const override;
	virtual void initScreen() override;
	virtual void destroyScreen() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update(double dt) override;
	virtual void draw() override;

  private:
	void initUI();
	void initShaders();

	bool EventBackButtonClicked(const CEGUI::EventArgs& e);

	CEGUI::PushButton* m_backButton = nullptr;

	BARE2D::Camera2D	   m_uiCamera;
	BARE2D::Window*		   m_window		  = nullptr;
	BARE2D::BARECEGUI*	   m_gui		  = nullptr;
	BARE2D::BasicRenderer* m_renderer	  = nullptr;
	BARE2D::FontRenderer*  m_fontRenderer = nullptr;

	float m_time = 0.0f;

	/// UI Below
	Slider* m_masterVolumeSlider = nullptr;
	Slider* m_soundsVolumeSlider = nullptr;
	Slider* m_musicVolumeSlider	 = nullptr;
	/// End of UI
};
