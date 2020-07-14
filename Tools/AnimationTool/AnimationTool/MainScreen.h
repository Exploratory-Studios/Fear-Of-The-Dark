#pragma once

#include <vector>

#include <IGameScreen.h>
#include <Window.h>
#include <SpriteBatch.h>
#include <DebugRenderer.h>
#include <GUI.h>
#include <Camera2D.h>

//#include <Code/XMLData.h>
#include "AugAnimation.h"

enum class ToolType {
	ROTATE,
	OFFSET,
	OFFSET_CENTRE
};

struct KeyFrame {
	std::vector<glm::vec2> offsets;
	std::vector<float> angles;
	std::vector<glm::vec2> centres; // Centres of rotation
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

		bool selectLimb(glm::vec2& mouseCoords);
		AnimationModule::SkeletalAnimation getSkeletal(); // Constructs the skeletal animation from existing data.
		void addFrame();

		void exportToFile(std::string filepath);
		void importFromFile(std::string filepath);

		// Actual functionality
		ToolType m_tool;
		bool m_previewing = false;
		std::vector<KeyFrame> m_keyframes;
		std::vector<AnimationModule::AugLimb> m_limbs;
		std::vector<bool> m_limbsVisibility;
		std::vector<unsigned int> m_limbIndices;
		std::vector<AnimationModule::AugAnimation> m_skins;
		AnimationModule::SkeletalAnimation m_currentSkeletal;
		unsigned int m_selectedLimb = 0;
		bool m_limbSelected = false;
		glm::vec2 m_grabPosition = glm::vec2(0.0f);
		glm::vec2 m_initialGrabPosition = glm::vec2(0.0f);
		unsigned int m_selectedFrame = 0;
		glm::vec2 m_skinSize = glm::vec2(32.0f, 64.0f);

		unsigned int m_frame = 0;

		// Rendering
		GLEngine::SpriteBatch m_spriteBatch;
		GLEngine::DebugRenderer m_debugRenderer;
		GLEngine::GLSLProgram m_textureProgram;
		GLEngine::GLSLProgram m_backgroundProgram;

		GLEngine::Camera2D m_cam;
		float m_scale = 1.0f;
		GLEngine::Camera2D m_backgroundCam;

		GLEngine::Window* m_window = nullptr;
		//XMLModule::TileData t;

		// GUI
		GLEngine::GUI m_gui;

		CEGUI::DefaultWindow* m_textLabel = nullptr;

		CEGUI::FrameWindow* m_fileFrame = nullptr;
		CEGUI::PushButton* m_exportButton = nullptr;
		CEGUI::Editbox* m_nameBox = nullptr;
		CEGUI::PushButton* m_importButton = nullptr;
		CEGUI::Editbox* m_skinBox = nullptr;
		CEGUI::Spinner* m_skinX = nullptr;
		CEGUI::Spinner* m_skinY = nullptr;
		CEGUI::PushButton* m_loadSkinButton = nullptr;
		CEGUI::PushButton* m_newLimbButton = nullptr;
		bool EventExportButtonClicked(const CEGUI::EventArgs& e);
		bool EventImportButtonClicked(const CEGUI::EventArgs& e);
		bool EventNewLimbButtonClicked(const CEGUI::EventArgs& e);

		CEGUI::FrameWindow* m_toolFrame = nullptr;
		CEGUI::PushButton* m_removeLimbButton = nullptr;
		CEGUI::PushButton* m_rotateOffsetButton = nullptr;
		CEGUI::PushButton* m_makeAllVisibleButton = nullptr;
		bool EventMakeAllVisibleButtonClicked(const CEGUI::EventArgs& e);
		bool EventRemoveLimbButtonClicked(const CEGUI::EventArgs& e);
		bool EventLoadSkinButtonClicked(const CEGUI::EventArgs& e);
		bool EventRotateOffsetLimbButtonClicked(const CEGUI::EventArgs& e);

		CEGUI::FrameWindow* m_keyframesFrame = nullptr;
		CEGUI::Listbox* m_keyframesList = nullptr;
		CEGUI::PushButton* m_playPauseButton = nullptr;
		CEGUI::PushButton* m_addFrameButton = nullptr;
		CEGUI::PushButton* m_removeFrameButton = nullptr;
		bool EventKeyFrameClicked(const CEGUI::EventArgs& e);
		bool EventPlayPauseButtonClicked(const CEGUI::EventArgs& e);
		bool EventAddFrameButtonClicked(const CEGUI::EventArgs& e);
		bool EventRemoveFrameButtonClicked(const CEGUI::EventArgs& e);

};
