#include "QuestTypes.h"

#include <fstream>

#include <LuaScript.hpp>
#include <LuaScriptQueue.hpp>
#include <LuaContextManager.hpp>

#include "CustomXMLTypes.h"

namespace QuestModule {
	Objective::Objective(std::string& instructions, BARE2D::LuaScript& script) {
		m_instructions		 = instructions;
		m_confirmationScript = script;
	}

	void Objective::start() {
		// Starts the confirmations script.
		m_confirmationScriptHandle = BARE2D::LuaScriptQueue::getInstance()->addLuaScript(m_confirmationScript);
	}
	bool Objective::update() {
		/// Checks if confirmation "script" finished.
		BARE2D::LuaScriptContextWrapper* confScript = BARE2D::LuaContextManager::getInstance()->getContext(m_confirmationScriptHandle);
		if(!confScript) return true;
		return confScript->isCompleted();
	}

	std::string Objective::getDisplayText() {
		return m_instructions;
	}

	Quest::Quest(unsigned int id) {
		XMLModule::QuestData data = getQuestData(id);

		std::vector<unsigned int> objectives = data.objectives;

		initObjectives(objectives);

		m_completionScript = data.completionScript;
	}

	void Quest::start() {
		startObjective(0);
		m_state = QuestState::CURRENT;
	}

	void Quest::update() {
		std::vector<unsigned int> current = getCurrentObjectives();

		for(unsigned int i = 0; i < current.size(); i++) {
			if(m_objectives[current[i]].update())
				m_objectiveStates[current[i]] = ObjectiveState::DONE;
		}

		if(getCurrentObjectives().size() == 0 && current.size() > 0) {
			// All objectives finished!
			if(!startObjective(current[current.size() - 1] + 1)) {
				finish();
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

	void Quest::finish() {
		/// Sets the state to DONE, executes Lua script.
		m_state					  = QuestState::DONE;
		BARE2D::LuaScriptQueue::getInstance()->addLuaScript(m_completionScript);
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

	bool Quest::startObjective(unsigned int i) {
		/// Starts an objective, checking if it exists first, and setting its state after. If no objective at `i` exists, returns false. Else, true
		// Check if it exists.
		if(i < m_objectives.size()) {
			// It does! Start it!
			m_objectives[i].start();
			m_objectiveStates[i] = ObjectiveState::CURRENT;
			return true;
		}

		return false;
	}

	void Quest::initObjectives(std::vector<unsigned int> objectiveData) {
		/// Initializes all objectives based on their Objective objects.
		for(unsigned int i = 0; i < objectiveData.size(); i++) {
			XMLModule::QuestObjectiveData d = getQuestObjectiveData(objectiveData[i]);

			std::string text = d.text;
			
			BARE2D::LuaScript confirmationScript = d.confirmationScript;

			m_objectives.emplace_back(text, confirmationScript);
			m_objectiveStates.emplace_back(ObjectiveState::HIDDEN);
		}
	}
} // namespace QuestModule
