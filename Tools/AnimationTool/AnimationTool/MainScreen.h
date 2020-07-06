#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <SpriteBatch.h>
#include <DebugRenderer.h>
#include <GUI.h>
#include <Camera2D.h>

//#include <Code/XMLData.h>

enum class ToolType {
	ROTATE,
	OFFSET
};


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
		virtual void onEntry() override;
		virtual void onExit() {}

		virtual void update() override;
		virtual void draw() override;

	private:
		void checkInput();
		void initUI();
		void initShaders();

		// Actual functionality
		ToolType m_tool;

		// Rendering
		GLEngine::SpriteBatch m_spriteBatch;
		GLEngine::DebugRenderer m_debugRenderer;
		GLEngine::GLSLProgram m_textureProgram;
		GLEngine::GLSLProgram m_backgroundProgram;

		GLEngine::Camera2D m_cam;

		GLEngine::Window* m_window = nullptr;
		//XMLModule::TileData t;

		// GUI
		GLEngine::GUI m_gui;

		CEGUI::FrameWindow* m_fileFrame = nullptr;
		CEGUI::PushButton* m_exportButton = nullptr;
		CEGUI::Editbox* m_nameBox = nullptr;
		CEGUI::PushButton* m_newLimbButton = nullptr;
		bool EventExportButtonClicked(const CEGUI::EventArgs& e);
		bool EventNewLimbButtonClicked(const CEGUI::EventArgs& e);

		CEGUI::FrameWindow* m_toolFrame = nullptr;
		CEGUI::PushButton* m_removeLimbButton = nullptr;
		CEGUI::PushButton* m_rotateOffsetButton = nullptr;
		bool EventRemoveLimbButtonClicked(const CEGUI::EventArgs& e);
		bool EventRotateOffsetLimbButtonClicked(const CEGUI::EventArgs& e);

		CEGUI::FrameWindow* m_keyframesFrame = nullptr;
		CEGUI::PushButton* m_playPauseButton = nullptr;
		CEGUI::PushButton* m_addFrameButton = nullptr;
		CEGUI::PushButton* m_removeFrameButton = nullptr;
		bool EventPlayPauseButtonClicked(const CEGUI::EventArgs& e);
		bool EventAddFrameButtonClicked(const CEGUI::EventArgs& e);
		bool EventRemoveFrameButtonClicked(const CEGUI::EventArgs& e);

};
