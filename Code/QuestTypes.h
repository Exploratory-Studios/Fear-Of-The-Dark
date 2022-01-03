#pragma once

#include <XMLDataManager.hpp>
#include <LuaScript.hpp>

namespace ScriptingModule {
	class Scripter;
	class LuaScript;
} // namespace ScriptingModule

namespace QuestModule {
	enum class ObjectiveState {
		DONE,	 // Already been done
		CURRENT, // In progress
		HIDDEN	 // Future objective, (often) hidden to make sure the player doesn't do it early.
	};

	enum class QuestState {
		DONE,	   // Already been done
		CURRENT,   // In progress
		UNASSIGNED // Not started yet
	};

	class Objective {
		/**
            Responsiblities:
             - Store text and Lua literal/script path, read scripts if necessary.
             - Make them available for read-only!
        **/
	  public:
		Objective(std::string& instructions, BARE2D::LuaScript& script);

		void start(); // Starts the confirmation "script", grabbing it's LuaScript pointer.
		bool update();							  // Checks if confirmation "script" finished.

		std::string getDisplayText();
		std::string getConfirmationLuaLiteral();

	  private:
		std::string m_instructions;

		BARE2D::LuaScript				m_confirmationScript;
		unsigned int m_confirmationScriptHandle = -1;
	};

	class Quest {
		/**
            Responsiblities:
             - Retrieve all objectives from XML based on a given ID.
             - Store Objectives in order
             - Store current objective status (ObjectiveState)
             - Store/set current state (QuestState)
        **/
	  public:
		Quest(unsigned int id);

		void start();
		void update();

		QuestState			   getState();
		std::vector<Objective> getObjectives(
			ObjectiveState state); // Returns a vector of objectives (copies) with visibility=visibilityLevel

	  private:
		void finish();

		std::vector<unsigned int> getCurrentObjectives();
		bool					  startObjective(unsigned int i);

		void initObjectives(std::vector<unsigned int> objectiveData);

		std::vector<Objective>		m_objectives;
		std::vector<ObjectiveState> m_objectiveStates;
		QuestState					m_state = QuestState::UNASSIGNED;
		BARE2D::LuaScript				m_completionScript;
	};
} // namespace QuestModule
