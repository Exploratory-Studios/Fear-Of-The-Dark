#include "Console.h"

Console::Console()
{
    //ctor
}

Console::~Console()
{
    //dtor
}

void Console::init(GLEngine::GUI& gui, ScriptQueue* sq) {
    m_sq = sq;

    m_frame = static_cast<CEGUI::FrameWindow*>(gui.createWidget("FOTDSkin/FrameWindow", glm::vec4(0.03f, 0.67f, 0.94f, 0.3f), glm::vec4(0.0f), "ConsoleFrameWindowMaster"));
    m_frame->setDragMovingEnabled(false);
    m_frame->setAlwaysOnTop(true);
    m_frame->setRollupEnabled(false);
    m_frame->setSizingEnabled(false);
    m_frame->setTitleBarEnabled(false);
    m_frame->setCloseButtonEnabled(false);

    m_scrollable = static_cast<CEGUI::ScrollablePane*>(gui.createWidget(m_frame, "FOTDSkin/ScrollablePane", glm::vec4(0.0f, 0.0f, 1.0f, 0.7f), glm::vec4(0.0f), "ConsoleScrollablePaneHistory"));

    m_historyLabel = static_cast<CEGUI::DefaultWindow*>(gui.createWidget(m_scrollable, "FOTDSkin/Label", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f), "ConsoleScrollableHistoryLabel"));

    m_editbox = static_cast<CEGUI::Editbox*>(gui.createWidget(m_frame, "FOTDSkin/Editbox", glm::vec4(0.0f, 0.7f, 1.0f, 0.3f), glm::vec4(0.0f), "ConsoleEditbox"));
    m_editbox->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Console::onEditboxInput, this));

    hide();

}

void Console::show() {
    m_frame->show();
    m_showing = true;
}

void Console::hide() {
    m_frame->hide();
    m_showing = false;
}

void Console::processCommand(std::string& command) {

    Script script;
    script.commands.push_back(command);

    unsigned int i = m_sq->addScript(script);
    m_sq->activateScript(i);

}

/// PRIVATE FUNCTIONS
#include <iostream>
bool Console::onEditboxInput(const CEGUI::EventArgs& e) {
    const CEGUI::KeyEventArgs& newArgs = static_cast<const CEGUI::KeyEventArgs&>(e);
    if(newArgs.scancode == CEGUI::Key::Return) {
        std::string command = m_editbox->getText().c_str();
        processCommand(command);
        hide();
        m_editbox->setText("");
        return true;
    } else if(newArgs.scancode == CEGUI::Key::Backspace) {
        std::string command = m_editbox->getText().c_str();
        command.pop_back();
        m_editbox->setText(command);
        return true;
    }
    return true;
}

