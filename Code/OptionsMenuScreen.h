#pragma once

#include "ScreenIndices.h"

#include "PresetValues.h"

#include "Options.h"

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>

class Slider {
		/// A wrapper class for a CEGUI::Slider and CEGUI::Label. Give it a title and a pointer to some data!
	public:
		Slider(GLEngine::GUI& gui, float* data, glm::vec4 destRect, std::string name, std::string shownName = "");

		void setFont(std::string font);

	private:
		void init(GLEngine::GUI& gui, glm::vec4& destRect, std::string& name, std::string& shownName);

		float* m_data = nullptr;

		CEGUI::Slider* m_slider = nullptr;
		CEGUI::DefaultWindow* m_label = nullptr;
};

class OptionsMenuScreen : public GLEngine::IGameScreen {
	public:
		OptionsMenuScreen(GLEngine::Window* window);
		virtual ~OptionsMenuScreen();

		virtual int getNextScreenIndex() const override;
		virtual int getPreviousScreenIndex() const override;
		virtual void build() override;
		virtual void destroy() override;
		virtual void onEntry() override;
		virtual void onExit() override;
		virtual void update() override;
		virtual void draw() override;

	private:
		void initUI();
		void initShaders();
		void checkInput();
		void updateMousebuttonDown(SDL_Event& evnt);

		bool EventBackButtonClicked(const CEGUI::EventArgs& e);

		CEGUI::PushButton* m_backButton = nullptr;

		GLEngine::Camera2D m_uiCamera;
		GLEngine::Window* m_window;
		GLEngine::GUI m_gui;
		GLEngine::SpriteBatch m_spriteBatch;
		GLEngine::GLSLProgram m_uiTextureProgram;
		GLEngine::SpriteFont m_spriteFont;

		float m_time = 0.0f;

		/// UI Below
		Slider* m_masterVolumeSlider = nullptr;
		Slider* m_soundsVolumeSlider = nullptr;
		Slider* m_musicVolumeSlider = nullptr;
		/// End of UI
};
