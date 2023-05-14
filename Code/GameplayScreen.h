#pragma once

#include <Screen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BumpyRenderer.hpp>
#include <TexturelessRenderer.hpp>
#include <DebugRenderer.hpp>
#include <ShaderProgram.hpp>
#include <FBORenderer.hpp>
#include <FontRenderer.hpp>
#include <Logger.hpp>
#include <ParticleEngine2D.hpp>
#include <InputManager.hpp>

#include <cstdio>

#include "Options.h"

#include "WorldIOManager.h"
#include "QuestManager.h"
#include "GameAudioManager.h"
#include "DialogueManager.h"

#include "ScreenIndices.h"
#include "Console.h"

enum class GameState { PAUSE, PLAY };

class Scripter;
class Console;
class Map;

class GameplayScreen : public BARE2D::Screen {
  public:
	GameplayScreen(BARE2D::Window* window, WorldIOManager* WorldIOManager, BARE2D::InputManager* input);
	virtual ~GameplayScreen();

	virtual int	 getNextScreenIndex() const override;
	virtual void initScreen() override;
	virtual void destroyScreen() override;
	virtual void onEntry() override;
	virtual void onExit() override;
	virtual void update(double dt) override;
	virtual void draw() override;

	void setCameraLocked(bool setting) {
		m_cameraLocked = setting;
	}

	void setSmoothMoveTarget(glm::vec2 target) {
		m_smoothMoveTarget = target;
	}
	void setSmoothMoveSpeed(float speed) {
		m_smoothMoveSpeed = speed;
	}

	void pauseForCutscene() {
		m_cutscenePause = true; // Sets the world to not update stuff, but still display
	}
	void unpauseCutscene() {
		m_cutscenePause = false; // Sets the world back to normal
	}

  private:
	void drawSky(); // These draw to whatever colour attachment that is active.
	void drawSunMoon();
	void drawWorld();
	void drawWorldSunlight();
	void drawParticles();
	void drawFluids();
	void drawGUI();

	void checkInput();
	void initUI();

	void tick();

	void drawHUD();
	void drawDebug();

	glm::vec4 getScreenBox();

	void pauseGame();	 // Inits pause UI and sets GameState
	void continueGame(); // Hides pause UI and sets GameState

	std::vector<CEGUI::Window*> m_pauseWidgets;
	bool						pause_resume_button_clicked(const CEGUI::EventArgs& e);
	bool						pause_save_button_clicked(const CEGUI::EventArgs& e);
	bool						pause_quit_button_clicked(const CEGUI::EventArgs& e);

	void updateScale();

	BARE2D::Window*			 m_window = nullptr;
	BARE2D::ParticleEngine2D m_particle2d;
	BARE2D::InputManager*	 m_inputManager = nullptr;

	BARE2D::FBORenderer* m_mainFBO = nullptr; // Used for pretty post processing.
	//BARE2D::FBORenderer* m_particleFBO = nullptr; // Used for partying particles!

	BARE2D::BasicRenderer* m_basicRenderer = nullptr; // used in drawing the main FBO
	BARE2D::BumpyRenderer* m_worldRenderer =
		nullptr; // Used in drawing both normal maps and regular collour for the world - tiles, entities, etc.
	BARE2D::BasicRenderer* m_skyRenderer = nullptr; // Used in rendering the background sky image
	BARE2D::BasicRenderer* m_sunlightRenderer =
		nullptr; // Used in rendering sunlight values to the main FBO's 2nd colour attachment (attachment #1)
	BARE2D::BasicRenderer* m_fluidRenderer = nullptr; // Used in rendering fluids to the screen.

	BARE2D::FontRenderer*  m_fontRenderer = nullptr;
	BARE2D::DebugRenderer* m_dr			  = nullptr;

	QuestModule::QuestManager*		 m_questManager	   = nullptr;
	DialogueModule::DialogueManager* m_dialogueManager = nullptr;
	WorldIOManager*					 m_WorldIOManager  = nullptr;
	GameAudioManager*				 m_audio		   = nullptr;
	Map*							 m_worldMap		   = nullptr;

	GameState m_gameState	  = GameState::PLAY;
	GameState m_lastGameState = GameState::PLAY;

	glm::vec2 m_smoothMoveTarget;		// Only used by camera, set only by ScripterMain
	float	  m_smoothMoveSpeed = 0.0f; // Only used by camera, set only by ScripterMain

	float m_time  = 0.0f; // Used for animations, NOT DAYLIGHT
	float m_frame = 0.0f;

	int m_lastSongPlayed = 0;

	float m_scale = MIN_ZOOM;

	bool m_cutscenePause =
		false; // This is a sort of 'soft' pause, meaning that only higher-level updating operations will be paused, such as collision, input, etc. (Blocks will still update like normal)

	bool m_debuggingInfo = false;

	glm::vec2 m_lastPlayerPos = glm::vec2(0.0f);

	Console* m_console = nullptr;

#ifdef DEV_CONTROLS
	CEGUI::DefaultWindow* m_fpsWidget = nullptr;

	bool m_debugBool = false;
#endif // DEV_CONTROLS

	std::vector<int> m_activatedChunks; // Chunks that should still update
	std::vector<int> m_drawnChunks;		// Chunks that should still be drawn
	int				 m_lastActivated = -1;

	bool m_cameraLocked = false; // Can the camera move with the player, or is it 'locked'?

	unsigned int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;

	float variable = 0.0f;
};
