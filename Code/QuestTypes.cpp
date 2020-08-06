#include "QuestTypes.h"

#include <fstream>

#include "LuaScript.h"
#include "ScripterMain.h"

namespace QuestModule {
	Objective::Objective(std::string& instructions, unsigned int& scriptID) {
		m_instructions = instructions;
		m_confirmationScript = scriptID;
	}

	void Objective::start(ScriptingModule::Scripter* scripter) {
		/// Starts the confirmation "script", grabbing it's LuaScript pointer.
		ScriptingModule::Script s = ScriptingModule::ScriptQueue::getScript(m_confirmationScript);
		m_confirmationScriptHandle = scripter->executeScript(s);
	}
	bool Objective::update() {
		/// Checks if confirmation "script" finished.
		if(m_confirmationScriptHandle->isFinished()) {
			return true;
		}
		return false;
	}

	std::string Objective::getDisplayText() {
		return m_instructions;
	}

	Quest::Quest(unsigned int id) {
		XMLModule::QuestData data = XMLModule::XMLData::getQuestData(id);

		std::vector<unsigned int> objectives = data.objectives;

		initObjectives(objectives);

		m_completionScript = data.completionScript.getID();
	}

	void Quest::start(ScriptingModule::Scripter* scripter) {
		startObjective(scripter, 0);
		m_state = QuestState::CURRENT;
	}

	void Quest::update(ScriptingModule::Scripter* scripter) {
		std::vector<unsigned int> current = getCurrentObjectives();

		for(unsigned int i = 0; i < current.size(); i++) {
			if(m_objectives[current[i]].update())
				m_objectiveStates[current[i]] = ObjectiveState::DONE;
		}

		if(getCurrentObjectives().size() == 0 && current.size() > 0) {
			// All objectives finished!
			if(!startObjective(scripter, current[current.size() - 1] + 1)) {
				finish(scripter);
			}
		}
	}

	QuestState Quest::getState() {
		return m_state;
	}

	std::vector<Objective> Quest::getObjectives(ObjectiveState state) {
		/// Returns a vector of objectives (copies) with visibility=visibilityLevel

		std::vector<Objective> ret;

		for(unsigned int i = 0; i < m_objectiveStates.size(); i++) {
			if(m_objectiveStates[i] == state) {
				ret.push_back(m_objectives[i]);
			}
		}

		return ret;

	}

	void Quest::finish(ScriptingModule::Scripter* scripter) {
		/// Sets the state to DONE, executes Lua script.
		m_state = QuestState::DONE;
		ScriptingModule::Script s = ScriptingModule::ScriptQueue::getScript(m_completionScript);
		scripter->executeScript(s);
	}

	std::vector<unsigned int> Quest::getCurrentObjectives() {
		/// Returns a vector of indices of objectives with state=CURRENT

		std::vector<unsigned int> ret;

		for(unsigned int i = 0; i < m_objectiveStates.size(); i++) {
			if(m_objectiveStates[i] == ObjectiveState::CURRENT) {
				ret.push_back(i);
			}
		}

		return ret;
	}

	bool Quest::startObjective(ScriptingModule::Scripter* scripter, unsigned int i) {
		/// Starts an objective, checking if it exists first, and setting its state after. If no objective at `i` exists, returns false. Else, true
		// Check if it exists.
		if(i < m_objectives.size()) {
			// It does! Start it!
			m_objectives[i].start(scripter);
			m_objectiveStates[i] = ObjectiveState::CURRENT;
			return true;
		}

		return false;
	}

	void Quest::initObjectives(std::vector<unsigned int> objectiveData) {
		/// Initializes all objectives based on their Objective objects.
		for(unsigned int i = 0; i < objectiveData.size(); i++) {
			XMLModule::QuestObjectiveData d = XMLModule::XMLData::getQuestObjectiveData(objectiveData[i]);

			std::string text = d.text;;
			unsigned int confirmationScript = d.confirmationScript.getID();

			m_objectives.emplace_back(text, confirmationScript);
			m_objectiveStates.emplace_back(ObjectiveState::HIDDEN);
		}
	}
}
