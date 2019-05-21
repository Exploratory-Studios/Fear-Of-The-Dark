#pragma once

#include "Scripting/ScripterMain.h"
#include <GUI.h>

class Scripter;

class Console
{
    public:
        Console();
        virtual ~Console();

        void init(GLEngine::GUI& gui, Scripter* scripter);

        void show();
        void hide();

        bool isShown() { return m_showing; }

        void processCommand(std::string& command);

    private:
        bool onEditboxInput(const CEGUI::EventArgs& e);
        bool onScrollableMouseWheel(const CEGUI::EventArgs& e);

        CEGUI::FrameWindow* m_frame = nullptr;
        CEGUI::ScrollablePane* m_scrollable = nullptr;

        std::vector<CEGUI::DefaultWindow*> m_historyLabels;
        std::vector<std::string> m_commandHistory;
        int m_historySelection = -1;

        CEGUI::Editbox* m_editbox = nullptr;

        bool m_showing = false;

        Scripter* m_scripter = nullptr;
        GLEngine::GUI* m_gui = nullptr;

};
