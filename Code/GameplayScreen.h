#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <DebugRenderer.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>

#include <cstdio>

#include "Options.h"

#include "WorldIOManager.h"
#include "QuestManager.h"
#include "AudioManager.h"
#include "ParticleEngine2D.h"
#include "DialogueManager.h"

#include "ScreenIndices.h"
#include "Logging.h"
#include "Console.h"

enum class GameState {
	PAUSE,
	PLAY
};

class Scripter;
class Console;

class GameplayScreen : public GLEngine::IGameScreen {
	public:
		GameplayScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager, World* world, Options* options);
		virtual ~GameplayScreen();

		virtual int getNextScreenIndex() const override;
		virtual int getPreviousScreenIndex() const override;
		virtual void build() override;
		virtual void destroy() override;
		virtual void onEntry() override;
		virtual void onExit() override;
		virtual void update() override;
		virtual void draw() override;

		void setCameraLocked(bool setting) {
			m_cameraLocked = setting;
		}
		GLEngine::Camera2D* getCamera() {
			return &m_camera;
		}

		void setSmoothMoveTarget(glm::vec2 target) {
			m_smoothMoveTarget = target;
		}
		void setSmoothMoveSpeed(float speed) {
			m_smoothMoveSpeed = speed;
		}

		void pauseForCutscene() {
			m_cutscenePause = true;    // Sets the world to not update stuff, but still display
		}
		void unpauseCutscene() {
			m_cutscenePause = false;    // Sets the world back to normal
		}

	private:
		void checkInput();
		void initShaders();
		void initUI();

		void tick();

		void drawHUD();
		void drawWorld();
		void drawDebug();

		glm::vec4 getScreenBox();

		void pauseGame(); // Inits pause UI and sets GameState
		void continueGame(); // Hides pause UI and sets GameState

		std::vector<CEGUI::Window*> m_pauseWidgets;
		bool pause_resume_button_clicked(const CEGUI::EventArgs& e);
		bool pause_save_button_clicked(const CEGUI::EventArgs& e);
		bool pause_quit_button_clicked(const CEGUI::EventArgs& e);

		void updateScale();

		GLEngine::Camera2D m_camera;
		GLEngine::Camera2D m_uiCamera;
		GLEngine::Window* m_window;
		GLEngine::GUI* m_gui = nullptr;
		GLEngine::SpriteBatch m_spriteBatch;
		GLEngine::ParticleEngine2D m_particle2d;

		GLEngine::FrameBufferObject m_mainFBO; // Used for post processing.
		GLEngine::FrameBufferObject m_skyFBO; // Used for the sky!
		GLEngine::FrameBufferObject m_particleFBO; // Used for particles!

		GLEngine::GLSLProgram m_textureProgram;
		GLEngine::GLSLProgram m_uiTextureProgram;
		GLEngine::GLSLProgram m_vignetteTextureProgram;
		GLEngine::GLSLProgram m_skyTextureProgram;
		GLEngine::GLSLProgram m_basicTextureProgram;
		GLEngine::GLSLProgram m_postProcessor;

		GLEngine::SpriteFont m_spriteFont;
		GLEngine::DebugRenderer m_dr;

		ScriptingModule::Scripter* m_scripter = nullptr;
		QuestModule::QuestManager* m_questManager = nullptr;
		DialogueModule::DialogueManager* m_dialogueManager = nullptr;
		WorldIOManager* m_WorldIOManager = nullptr;
		World* m_world = nullptr;
		AudioManager* m_audio = nullptr;

		GameState m_gameState = GameState::PLAY;

		glm::vec2 m_smoothMoveTarget; // Only used by camera, set only by ScripterMain
		float m_smoothMoveSpeed = 0.0f; // Only used by camera, set only by ScripterMain

		float m_time = 0.0f; // Used for animations, NOT DAYLIGHT
		float m_frame = 0.0f;
		float m_deltaTime = 1.0f;

		int m_lastSongPlayed = 0;

		float m_scale = MIN_ZOOM;

		bool m_cutscenePause = false; // This is a sort of 'soft' pause, meaning that only higher-level updating operations will be paused, such as collision, input, etc. (Blocks will still update like normal)

		bool m_debuggingInfo = false;

		glm::vec2 m_lastPlayerPos;

		Logger* logger = Logger::getInstance();

		Console* m_console = nullptr;

#ifdef DEV_CONTROLS
		CEGUI::DefaultWindow* m_fpsWidget = nullptr;
#endif // DEV_CONTROLS

		std::vector<int> m_activatedChunks; // Chunks that should still update
		std::vector<int> m_drawnChunks; // Chunks that should still be drawn
		int m_lastActivated = -1;

		bool m_cameraLocked = false; // Can the camera move with the player, or is it 'locked'?

		unsigned int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;

		bool m_hasBeenInited = false;

		Options* m_options = nullptr; // A struct that holds all options. A struct is used so that it can be passed from screen to screen.
};
