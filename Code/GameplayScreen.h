#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>

#include "WorldManager.h"

enum class GameState {
    PAUSE,
    PLAY
};

class GameplayScreen : public GLEngine::IGameScreen
{
    public:
        GameplayScreen(GLEngine::Window* window, WorldManager* worldManager);
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

        GLEngine::Camera2D m_camera;
        GLEngine::Camera2D m_uiCamera;
        GLEngine::Window* m_window;
        GLEngine::GUI m_gui;
        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::GLSLProgram m_textureProgram;
        GLEngine::SpriteFont m_spriteFont;

        WorldManager* m_worldManager = nullptr;

        GameState m_gameState = GameState::PLAY;

        Entity testEnt;

        float m_time = 0.0f;
};
