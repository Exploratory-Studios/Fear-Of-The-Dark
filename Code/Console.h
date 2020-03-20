#pragma once

#include "Scripting/ScripterMain.h"
#include <GUI.h>

class Scripter;
class World;
class GameplayScreen;

class Console
{
    public:
        Console();
        virtual ~Console();

        void init(GLEngine::GUI& gui, Scripter* scripter, World* world, QuestManager* qm, GameplayScreen* gs); /// TODO: Make this not have a world member or qm variable

        void show();
        void hide();

        bool isShown() { return m_showing; }

        void processCommand(World* world, QuestManager* qm, GameplayScreen* gs, std::string& command);

    private:
        bool onEditboxInput(const CEGUI::EventArgs& e);
        bool onScrollableMouseWheel(const CEGUI::EventArgs& e);

        CEGUI::FrameWindow* m_frame = nullptr;
        CEGUI::ScrollablePane* m_scrollable = nullptr;

        std::vector<CEGUI::DefaultWindow*> m_historyLabels;
        std::vector<std::string> m_commandHistory;
        unsigned int m_historySelection = (unsigned int)-1;

        CEGUI::Editbox* m_editbox = nullptr;

        World* m_world = nullptr;
        QuestManager* m_qm = nullptr;
        GameplayScreen* m_gs = nullptr;

        bool m_showing = false;

        Scripter* m_scripter = nullptr;
        GLEngine::GUI* m_gui = nullptr;

};
