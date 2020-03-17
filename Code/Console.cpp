#include "Console.h"

Console::Console()
{
    //ctor
}

Console::~Console()
{
    //dtor
}

void Console::init(GLEngine::GUI& gui, Scripter* scripter, World* world, QuestManager* qm) {
    m_scripter = scripter;
    m_world = world;
    m_qm = qm;

    m_frame = static_cast<CEGUI::FrameWindow*>(gui.createWidget("FOTDSkin/FrameWindow", glm::vec4(0.03f, 0.67f, 0.94f, 0.3f), glm::vec4(0.0f), "ConsoleFrameWindowMaster"));
    m_frame->setDragMovingEnabled(false);
    m_frame->setAlwaysOnTop(true);
    m_frame->setRollupEnabled(false);
    m_frame->setSizingEnabled(false);
    m_frame->setTitleBarEnabled(false);
    m_frame->setCloseButtonEnabled(false);

    m_scrollable = static_cast<CEGUI::ScrollablePane*>(gui.createWidget(m_frame, "FOTDSkin/ScrollablePane", glm::vec4(0.01f, 0.05f, 0.98f, 0.6f), glm::vec4(0.0f), "ConsoleScrollablePaneHistory"));
    m_scrollable->setContentPaneAutoSized(true);
    m_scrollable->setShowVertScrollbar(true);
    m_scrollable->subscribeEvent(CEGUI::ScrollablePane::EventMouseWheel, CEGUI::Event::Subscriber(&Console::onScrollableMouseWheel, this));

    m_editbox = static_cast<CEGUI::Editbox*>(gui.createWidget(m_frame, "FOTDSkin/Editbox", glm::vec4(0.01f, 0.7f, 0.98f, 0.25f), glm::vec4(0.0f), "ConsoleEditbox"));
    m_editbox->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Console::onEditboxInput, this));

    m_gui = &gui;

    hide();

}

void Console::show() {
    m_frame->show();
    m_editbox->activate();
    m_showing = true;
}

void Console::hide() {
    m_frame->hide();
    m_showing = false;
}

void Console::processCommand(World* world, QuestManager* qm, std::string& command) {
    /*std::string ret = m_scripter->executeCommand(world, qm, nullptr, command);

    std::vector<std::string> returnVector;

    std::string temp;

    for(unsigned int i = 0; i < ret.size(); i++) {
        if(ret[i] == '\n') {
            returnVector.push_back(temp);
            temp = "";
        } else {
            temp += ret[i];
        }
    }

    for(unsigned int i = 0; i < returnVector.size(); i++) {
        glm::vec4 destRect = glm::vec4(0.0f, m_historyLabels.size() * 0.3f, 1.0f, 0.3f);
        std::string name = std::string("ConsoleScrollableHistoryLabel") + std::to_string((unsigned int)m_historyLabels.size()) + std::string("_") + std::to_string(i);
        m_historyLabels.push_back(static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(m_scrollable, "FOTDSkin/Label", destRect, glm::vec4(0.0f), name)));
        m_historyLabels[m_historyLabels.size()-1]->setText("[vert-alignment='center']" + returnVector[i]);
        m_historyLabels[m_historyLabels.size()-1]->setProperty("HorzFormatting", "LeftAligned");
    }

    m_commandHistory.push_back(command);
    if(m_commandHistory.size() > 25) {
        for(int i = m_commandHistory.size()-1; i > 0; i--) {
            m_commandHistory[i-1] = m_commandHistory[i];
        }
        m_commandHistory.pop_back();
    }
    m_historySelection = m_commandHistory.size();*/
}

/// PRIVATE FUNCTIONS

bool Console::onEditboxInput(const CEGUI::EventArgs& e) {
    const CEGUI::KeyEventArgs& newArgs = static_cast<const CEGUI::KeyEventArgs&>(e);
    if(newArgs.scancode == CEGUI::Key::Return) {
        std::string command = m_editbox->getText().c_str();
        processCommand(m_world, m_qm, command);
        m_editbox->setText("");
        m_scrollable->setVerticalScrollPosition(1.0f);
        return true;
    } else if(newArgs.scancode == CEGUI::Key::Backspace) {
        std::string command = m_editbox->getText().c_str();
        if(command.size() > 0) command.pop_back();
        m_editbox->setText(command);
        return true;
    } else if(newArgs.scancode == CEGUI::Key::Backslash) {
        hide();
    } else if(newArgs.scancode == CEGUI::Key::ArrowUp) {
        if(m_historySelection > 0 && m_commandHistory.size() > 0) {
            m_historySelection--;
            m_editbox->setText(m_commandHistory[m_historySelection]);
        }
    } else if(newArgs.scancode == CEGUI::Key::ArrowDown) {
        if(m_historySelection < m_commandHistory.size()) m_historySelection++;
        if(m_historySelection == m_commandHistory.size()) m_editbox->setText("");
        if(m_historySelection < m_commandHistory.size()) {
            m_editbox->setText(m_commandHistory[m_historySelection]);
        }
    }
    return true;
}
#include <iostream>
bool Console::onScrollableMouseWheel(const CEGUI::EventArgs& e) {
    const CEGUI::MouseEventArgs& newArgs = static_cast<const CEGUI::MouseEventArgs&>(e);

    m_scrollable->setVerticalScrollPosition(m_scrollable->getVerticalScrollPosition() + m_scrollable->getHeight().d_scale / m_historyLabels.size() * newArgs.wheelChange);

    return true;
}
