#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>

#include <cstdio>

#include "WorldIOManager.h"
#include "WorldManager.h"

enum class GameState {
    PAUSE,
    PLAY
};

class GameplayScreen : public GLEngine::IGameScreen
{
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

    private:
        void checkInput();
        void initShaders();
        void initUI();

        void tick();

        void drawHUD();
        void drawWorld();
        void drawDebug();

        void updateScale();

        GLEngine::Camera2D m_camera;
        GLEngine::Camera2D m_uiCamera;
        GLEngine::Window* m_window;
        GLEngine::GUI m_gui;
        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::GLSLProgram m_textureProgram;
        GLEngine::GLSLProgram m_uiTextureProgram;
        GLEngine::SpriteFont m_spriteFont;
        GLEngine::DebugRenderer m_dr;

        WorldIOManager* m_WorldIOManager = nullptr;
        WorldManager m_worldManager;

        GameState m_gameState = GameState::PLAY;

        float m_time = 0.0f;
        float m_tickTime = 0.0f;
        float m_frame = 0.0f;
        float m_deltaTime = 1.0f;
        int m_tickRate = 4;

        float m_scale = 1.0f;

        bool m_debuggingInfo = false;

        glm::vec2 m_lastPlayerPos;
};
