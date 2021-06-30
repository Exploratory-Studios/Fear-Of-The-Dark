#pragma once

#include "ScripterMain.h"
#include <GUI.h>

namespace ScriptingModule {
	class Scripter;
}
namespace QuestModule {
	class QuestManager;
}
class World;
class GameplayScreen;

class Console {
  public:
	Console();
	virtual ~Console();

	void init(ScriptingModule::Scripter* scripter,
			  QuestModule::QuestManager* qm,
			  GameplayScreen*			 gs); /// TODO: Make this not have a world member or qm variable

	void show();
	void hide();

	bool isShown() {
		return m_showing;
	}

	void processCommand(QuestModule::QuestManager* qm, GameplayScreen* gs, std::string& command);

  private:
	bool onEditboxInput(const CEGUI::EventArgs& e);
	bool onScrollableMouseWheel(const CEGUI::EventArgs& e);

	CEGUI::FrameWindow*	   m_frame		= nullptr;
	CEGUI::ScrollablePane* m_scrollable = nullptr;

	std::vector<CEGUI::DefaultWindow*> m_historyLabels;
	std::vector<std::string>		   m_commandHistory;
	unsigned int					   m_historySelection = (unsigned int)-1;

	CEGUI::Editbox* m_editbox = nullptr;

	QuestModule::QuestManager* m_qm = nullptr;
	GameplayScreen*			   m_gs = nullptr;

	bool m_showing = false;

	ScriptingModule::Scripter* m_scripter = nullptr;
};
