#include "Console.h"

#include "Singletons.h"

Console::Console() {
	//ctor
}

Console::~Console() {
	//dtor
}

void Console::init(QuestModule::QuestManager* qm, GameplayScreen* gs) {
	m_qm = qm;
	m_gs = gs;

	m_frame = static_cast<CEGUI::FrameWindow*>(Singletons::getGUI()->createWidget("FOTDSkin/FrameWindow",
																				  glm::vec4(0.03f, 0.67f, 0.94f, 0.3f),
																				  glm::vec4(0.0f),
																				  nullptr,
																				  "ConsoleFrameWindowMaster"));
	m_frame->setDragMovingEnabled(false);
	m_frame->setAlwaysOnTop(true);
	m_frame->setRollupEnabled(false);
	m_frame->setSizingEnabled(false);
	m_frame->setTitleBarEnabled(false);
	m_frame->setCloseButtonEnabled(false);

	m_scrollable =
		static_cast<CEGUI::ScrollablePane*>(Singletons::getGUI()->createWidget("FOTDSkin/ScrollablePane",
																			   glm::vec4(0.01f, 0.05f, 0.98f, 0.6f),
																			   glm::vec4(0.0f),
																			   m_frame,
																			   "ConsoleScrollablePaneHistory"));
	m_scrollable->setContentPaneAutoSized(true);
	m_scrollable->setShowVertScrollbar(true);
	m_scrollable->subscribeEvent(CEGUI::ScrollablePane::EventMouseWheel,
								 CEGUI::Event::Subscriber(&Console::onScrollableMouseWheel, this));

	m_editbox = static_cast<CEGUI::Editbox*>(Singletons::getGUI()->createWidget("FOTDSkin/Editbox",
																				glm::vec4(0.01f, 0.7f, 0.98f, 0.25f),
																				glm::vec4(0.0f),
																				m_frame,
																				"ConsoleEditbox"));
	m_editbox->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&Console::onEditboxInput, this));

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

void Console::processCommand(QuestModule::QuestManager* qm, GameplayScreen* gs, std::string& command) {
	BARE2D::LuaScriptQueue::getInstance()->addLuaScript(command);
}

/// PRIVATE FUNCTIONS

bool Console::onEditboxInput(const CEGUI::EventArgs& e) {
	const CEGUI::KeyEventArgs& newArgs = static_cast<const CEGUI::KeyEventArgs&>(e);
	if(newArgs.scancode == CEGUI::Key::Return) {
		std::string command = m_editbox->getText().c_str();
		processCommand(m_qm, m_gs, command);
		m_editbox->setText("");
		m_scrollable->setVerticalScrollPosition(1.0f);
		return true;
	} else if(newArgs.scancode == CEGUI::Key::Backspace) {
		std::string command = m_editbox->getText().c_str();
		if(command.size() > 0)
			command.pop_back();
		m_editbox->setText(command);
		return true;
	} else if(newArgs.scancode == CEGUI::Key::ArrowUp) {
		if(m_historySelection > 0 && m_commandHistory.size() > 0) {
			m_historySelection--;
			m_editbox->setText(m_commandHistory[m_historySelection]);
		}
	} else if(newArgs.scancode == CEGUI::Key::ArrowDown) {
		if(m_historySelection < m_commandHistory.size())
			m_historySelection++;
		if(m_historySelection == m_commandHistory.size())
			m_editbox->setText("");
		if(m_historySelection < m_commandHistory.size()) {
			m_editbox->setText(m_commandHistory[m_historySelection]);
		}
	}
	return true;
}
#include <iostream>
bool Console::onScrollableMouseWheel(const CEGUI::EventArgs& e) {
	const CEGUI::MouseEventArgs& newArgs = static_cast<const CEGUI::MouseEventArgs&>(e);

	m_scrollable->setVerticalScrollPosition(m_scrollable->getVerticalScrollPosition() +
											m_scrollable->getHeight().d_scale / m_historyLabels.size() *
												newArgs.wheelChange);

	return true;
}
