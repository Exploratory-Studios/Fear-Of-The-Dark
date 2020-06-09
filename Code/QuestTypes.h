#pragma once

#include "XMLData.h"

#include "LuaScript.h"

namespace ScriptingModule { class Scripter; class LuaScript; }

namespace QuestModule {
    enum class ObjectiveState {
        DONE, // Already been done
        CURRENT, // In progress
        HIDDEN // Future objective, (often) hidden to make sure the player doesn't do it early.
    };

    enum class QuestState {
        DONE, // Already been done
        CURRENT, // In progress
        UNASSIGNED // Not started yet
    };

    class Objective {
        /**
            Responsiblities:
             - Store text and Lua literal/script path, read scripts if necessary.
             - Make them available for read-only!
        **/
        public:
            Objective(std::string& instructions, std::string& confirmationLuaLiteralOrPath, bool luaInFile = false);

            void start(ScriptingModule::Scripter* scripter); // Starts the confirmation "script", grabbing it's LuaScript pointer.
            bool update(); // Checks if confirmation "script" finished.

            std::string getDisplayText();
            std::string getConfirmationLuaLiteral();
        private:
            std::string m_instructions;
            std::string m_luaLiteral; // This is always the literal Lua code. The constructor reads the file (if one is provided) and puts the text in here.

            ScriptingModule::LuaScript* m_confirmationScript = nullptr;
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

            void start(ScriptingModule::Scripter* scripter);
            void update(ScriptingModule::Scripter* scripter);

            QuestState getState();
            std::vector<Objective> getObjectives(ObjectiveState state); // Returns a vector of objectives (copies) with visibility=visibilityLevel

        private:
            void finish(ScriptingModule::Scripter* scripter);

            std::vector<unsigned int> getCurrentObjectives();
            bool startObjective(ScriptingModule::Scripter* scripter, unsigned int i);

            void initObjectives(std::vector<XML_QuestObjectiveData> objectiveData);

            std::vector<Objective> m_objectives;
            std::vector<ObjectiveState> m_objectiveStates;
            QuestState m_state = QuestState::UNASSIGNED;
            std::string m_completionLuaLiteral; // A Lua literal string to run on completion.
    };
}
