#pragma once

#include <IGameScreen.h>
#include <Camera2D.h>
#include <GUI.h>

#include "WorldIOManager.h"
#include "ScreenIndices.h"

class WaitScreen : public GLEngine::IGameScreen
{

    public:
        WaitScreen(GLEngine::Window* window, WorldIOManager* worldIOManager);
        virtual ~WaitScreen();

        virtual int getNextScreenIndex() const override;
        virtual int getPreviousScreenIndex() const override;
        virtual void build() override;
        virtual void destroy() override;
        virtual void onEntry() override;
        virtual void onExit() override;
        virtual void update() override;
        virtual void draw() override;

    protected:
        void initShaders();
        void initUI();

        void checkInput();

        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::SpriteFont m_spriteFont;

        GLEngine::Camera2D m_uiCamera;
        GLEngine::Camera2D m_camera;

        GLEngine::GUI m_gui;

        GLEngine::GLSLProgram m_textureProgram;

        WorldIOManager* m_worldIOManager = nullptr;
        GLEngine::Window* m_window = nullptr;

        CEGUI::ProgressBar* m_progressBar = nullptr;
        CEGUI::DefaultWindow* m_messageBox = nullptr;

        float m_time = 0.0f;

        int m_nextScreenIndex = SCREEN_INDEX_GAMEPLAY;

};
