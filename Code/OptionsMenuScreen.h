#pragma once

#include "ScreenIndices.h"

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>

class OptionsMenuScreen : public GLEngine::IGameScreen
{
    public:
        OptionsMenuScreen(GLEngine::Window* window);
        virtual ~OptionsMenuScreen();

        virtual int getNextScreenIndex() const override;
        virtual int getPreviousScreenIndex() const override;
        virtual void build() override;
        virtual void destroy() override;
        virtual void onEntry() override;
        virtual void onExit() override;
        virtual void update() override;
        virtual void draw() override;

    private:
        void initUI();
        void initShaders();
        void checkInput();
        void updateMousebuttonDown(SDL_Event& evnt);

        bool EventBackButtonClicked(const CEGUI::EventArgs& e);

        CEGUI::PushButton* m_backButton = nullptr;

        GLEngine::Camera2D m_uiCamera;
        GLEngine::Window* m_window;
        GLEngine::GUI m_gui;
        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::GLSLProgram m_uiTextureProgram;
        GLEngine::SpriteFont m_spriteFont;

        float m_time = 0.0f;
};
