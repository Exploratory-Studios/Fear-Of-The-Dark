#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>
#include <ParticleEngine2D.h>

#include <cstdio>

#include "WorldIOManager.h"
//#include "WorldManager.h"

#include "ScreenIndices.h"
#include "Logging.h"
#include "Console.h"

enum class GameState {
    PAUSE,
    PLAY
};

class Scripter;
class Console;

class GameplayScreen : public GLEngine::IGameScreen
{
    friend class Scripter;

    public:
        GameplayScreen(GLEngine::Window* window, WorldIOManager* WorldIOManager);
        virtual ~GameplayScreen();

        virtual int getNextScreenIndex() const override;
        virtual int getPreviousScreenIndex() const override;
        virtual void build() override;
        virtual void destroy() override;
        virtual void onEntry() override;
        virtual void onExit() override;
        virtual void update() override;
        virtual void draw() override;

        void pauseForCutscene() { m_cutscenePause = true; } // Sets the world to not update stuff, but still display
        void unpauseCutscene() { m_cutscenePause = false; } // Sets the world back to normal

    private:
        void checkInput();
        void initShaders();
        void initUI();

        void tick();

        void drawHUD();
        void drawWorld();
        void drawDebug();

        void pauseGame(); // Inits pause UI and sets GameState
        void continueGame(); // Hides pause UI and sets GameState

        std::vector<CEGUI::Window*> m_pauseWidgets;
        bool pause_resume_button_clicked(const CEGUI::EventArgs& e);
        bool pause_save_button_clicked(const CEGUI::EventArgs& e);
        bool pause_quit_button_clicked(const CEGUI::EventArgs& e);

        void updateScale();
        void activateChunks();

        GLEngine::Camera2D m_camera;
        GLEngine::Camera2D m_uiCamera;
        GLEngine::Window* m_window;
        GLEngine::GUI m_gui;
        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::GLSLProgram m_textureProgram;
        GLEngine::GLSLProgram m_uiTextureProgram;
        GLEngine::GLSLProgram m_vignetteTextureProgram;
        GLEngine::GLSLProgram m_skyTextureProgram;
        GLEngine::SpriteFont m_spriteFont;
        GLEngine::DebugRenderer m_dr;
        GLEngine::ParticleEngine2D m_particle2d;

        Scripter* m_scripter = nullptr;
        QuestManager* m_questManager = nullptr;

        WorldIOManager* m_WorldIOManager = nullptr;
        //WorldManager m_worldManager;

        Player* m_player = nullptr;

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

        glm::vec2 m_lastSelectedPosition; // Only used when saving structures
        bool m_selecting = false;

        bool m_cameraLocked = false; // Can the camera move with the player, or is it 'locked'?

        unsigned int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;
};

#include "Scripting/ScripterMain.h"
#include "Console.h"
