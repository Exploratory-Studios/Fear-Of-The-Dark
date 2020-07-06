#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <SpriteBatch.h>
#include <DebugRenderer.h>
#include <GUI.h>

//#include <Code/XMLData.h>

class MainScreen : public GLEngine::IGameScreen {
	public:
		MainScreen(GLEngine::Window* window);
		~MainScreen();

		virtual int getNextScreenIndex() const {
			return 0;
		}
		virtual int getPreviousScreenIndex() const {
			return 0;
		}

		virtual void build() override;
		virtual void destroy() {}
		virtual void onEntry() {}
		virtual void onExit() {}

		virtual void update() override;
		virtual void draw() override;

	private:
		void checkInput();
		void initUI();
		void initShaders();

		// Rendering
		GLEngine::SpriteBatch m_spriteBatch;
		GLEngine::DebugRenderer m_debugRenderer;
		GLEngine::GLSLProgram m_textureProgram;

		//XMLModule::TileData t;

		// GUI
		GLEngine::GUI m_gui;

};
