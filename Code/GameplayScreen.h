#pragma once

#include <Screen.hpp>
#include <Window.hpp>
#include <Camera2D.hpp>
#include <BARECEGUI.hpp>
#include <BumpyRenderer.hpp>
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

class GameplayScreen : public BARE2D::Screen
{
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

	void setCameraLocked(bool setting)
	{
		m_cameraLocked = setting;
	}

	void setSmoothMoveTarget(glm::vec2 target)
	{
		m_smoothMoveTarget = target;
	}
	void setSmoothMoveSpeed(float speed)
	{
		m_smoothMoveSpeed = speed;
	}

	void pauseForCutscene()
	{
		m_cutscenePause = true; // Sets the world to not update stuff, but still display
	}
	void unpauseCutscene()
	{
		m_cutscenePause = false; // Sets the world back to normal
	}

private:
	void drawSkyToFBO(); /// These just draw to the FBOs, not to the screen.
	void drawWorldToFBO();
	void drawWorldSunlightToFBO();
	void drawParticlesToFBO();
	void drawFluidsToFBO();
	void drawGUIToScreen(); // These two actually do draw to the screen.
	void drawPostToScreen();

	void checkInput();
	void initUI();

	void tick();

	void drawHUD();
	void drawWorld();
	void drawDebug();

	glm::vec4 getScreenBox();

	void pauseGame();	 // Inits pause UI and sets GameState
	void continueGame(); // Hides pause UI and sets GameState

	std::vector<CEGUI::Window*> m_pauseWidgets;
	bool						pause_resume_button_clicked(const CEGUI::EventArgs& e);
	bool						pause_save_button_clicked(const CEGUI::EventArgs& e);
	bool						pause_quit_button_clicked(const CEGUI::EventArgs& e);

	void updateScale();

	BARE2D::Camera2D		 m_uiCamera;
	BARE2D::Window*			 m_window	= nullptr;
	BARE2D::BumpyRenderer*	 m_renderer = nullptr;
	BARE2D::ParticleEngine2D m_particle2d;
	BARE2D::InputManager* m_inputManager = nullptr;

	BARE2D::FBORenderer* m_mainFBO	   = nullptr; // Used for pretty post processing.
	BARE2D::FBORenderer* m_normalFBO   = nullptr; // Used for noob-y normal mapping.
	BARE2D::FBORenderer* m_skyFBO	   = nullptr; // Used for the spectacular sky!
	BARE2D::FBORenderer* m_particleFBO = nullptr; // Used for partying particles!
	BARE2D::FBORenderer* m_sunlightFBO = nullptr; // Used for seratonin-inducing sunlight!
	BARE2D::FBORenderer* m_fluidFBO	   = nullptr; // Used for fantastic fluids!

	BARE2D::FontRenderer*  m_fontRenderer = nullptr;
	BARE2D::DebugRenderer* m_dr			  = nullptr;

	QuestModule::QuestManager*		 m_questManager	   = nullptr;
	DialogueModule::DialogueManager* m_dialogueManager = nullptr;
	WorldIOManager*					 m_WorldIOManager  = nullptr;
	GameAudioManager*				 m_audio		   = nullptr;

	GameState m_gameState	  = GameState::PLAY;
	GameState m_lastGameState = GameState::PLAY;

	glm::vec2 m_smoothMoveTarget;		// Only used by camera, set only by ScripterMain
	float	  m_smoothMoveSpeed = 0.0f; // Only used by camera, set only by ScripterMain

	float m_time	  = 0.0f; // Used for animations, NOT DAYLIGHT
	float m_frame	  = 0.0f;

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
