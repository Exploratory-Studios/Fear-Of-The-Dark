#pragma once

#include <IGameScreen.h>
#include <Window.h>
#include <Camera2D.h>
#include <GUI.h>
#include <SpriteBatch.h>
#include <GLSLProgram.h>
#include <SpriteFont.h>

#include "ScreenIndices.h"
#include "WidgetLabel.h"
#include "WorldManager.h"

enum class MiniScreen {
    MAIN,
    SETTINGS,
    CREATE
};

class MiniScreenEntry {
    public:
        MiniScreenEntry(CEGUI::Window* widget, MiniScreen miniScreen) : m_widget(widget), m_miniScreen(miniScreen) {}
        void checkScreen(MiniScreen currentMiniScreen) {
                                                        if(currentMiniScreen == m_miniScreen) {
                                                            m_widget->enable();
                                                            m_widget->setVisible(true);
                                                        } else {
                                                            m_widget->disable();
                                                            m_widget->setVisible(false);
                                                        }
                                                       }

    private:
        CEGUI::Window* m_widget;
        MiniScreen m_miniScreen;
};

class LoadScreen : public GLEngine::IGameScreen
{
    public:
        LoadScreen(GLEngine::Window* window, WorldManager* worldManager);
        virtual ~LoadScreen();

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
        void initUI();
        void initShaders();

        bool onNewWorldCreateNewButtonClicked(const CEGUI::EventArgs& e);
        bool onCreateButtonClicked(const CEGUI::EventArgs& e);

        int m_nextScreenIndex = SCREEN_INDEX_NO_SCREEN;

        std::vector<WidgetLabel> m_labels;

        std::vector<MiniScreenEntry> m_miniScreenEntries;
        MiniScreen m_miniScreen = MiniScreen::MAIN;

        float m_time = 0;

        CEGUI::PushButton* m_createButton = nullptr;
        CEGUI::PushButton* m_newWorldCreateNewButton = nullptr;
        CEGUI::FrameWindow* m_newWorldFramewindow = nullptr; /// World creation buttons
        CEGUI::Editbox* m_newWorldNameEditbox = nullptr;
        CEGUI::Editbox* m_newWorldSeedEditbox = nullptr;

        GLEngine::Camera2D m_camera;
        GLEngine::Camera2D m_uiCamera;
        GLEngine::Window* m_window;
        GLEngine::GUI m_gui;
        GLEngine::SpriteBatch m_spriteBatch;
        GLEngine::GLSLProgram m_textureProgram;
        GLEngine::SpriteFont m_spriteFont;

        WorldManager* m_worldManager = nullptr;
};