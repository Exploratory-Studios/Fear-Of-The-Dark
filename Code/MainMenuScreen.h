#pragma once

#include "ScreenIndices.h"
#include "WidgetLabel.h"

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>
#include <Vertex.h>
#include <ResourceManager.h>

void addBackgroundImage(std::vector<GLEngine::GLTexture>& backgroundImages, std::string filename);

enum class NextScreen {
    OPTIONS,
    PLAY,
    EXIT
};

class MainMenuScreen : public GLEngine::IGameScreen
{
    public:
        MainMenuScreen(GLEngine::Window* window);
        virtual ~MainMenuScreen();

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

        bool EventPlayButtonClicked(const CEGUI::EventArgs& e);
        bool EventOptionsButtonClicked(const CEGUI::EventArgs& e);
        bool EventExitButtonClicked(const CEGUI::EventArgs& e);

        CEGUI::PushButton* m_playButton = nullptr;
        CEGUI::PushButton* m_optionsButton = nullptr;
        CEGUI::PushButton* m_exitButton = nullptr;

        GLEngine::Camera2D m_camera;
        GLEngine::Camera2D m_uiCamera;
        GLEngine::Window* m_window;
        GLEngine::GUI m_gui;
        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::GLSLProgram m_uiTextureProgram;
        GLEngine::SpriteFont m_spriteFont;

        std::vector<WidgetLabel> m_labels;

        std::vector<GLEngine::GLTexture> m_backgroundImages;
        GLEngine::ColourRGBA8 m_backgroundImageColour = GLEngine::ColourRGBA8(255, 255, 255, 50);
        int m_currentBackgroundImage;

        float m_time = 0.0f;
        float m_backgroundChangeTime = 15.0f;

        int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;
};
