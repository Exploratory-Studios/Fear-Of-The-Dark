#pragma once

#include "Scripting/ScriptQueue.h"

#include <GUI.h>

class Console
{
    public:
        Console();
        virtual ~Console();

        void init(GLEngine::GUI& gui, ScriptQueue* sq);

        void show();
        void hide();

        bool isShown() { return m_showing; }

        void processCommand(std::string& command);

    private:
        bool onEditboxInput(const CEGUI::EventArgs& e);

        CEGUI::FrameWindow* m_frame = nullptr;
        CEGUI::ScrollablePane* m_scrollable = nullptr;
        CEGUI::DefaultWindow* m_historyLabel = nullptr;
        CEGUI::Editbox* m_editbox = nullptr;

        bool m_showing = false;

        ScriptQueue* m_sq = nullptr;

};
