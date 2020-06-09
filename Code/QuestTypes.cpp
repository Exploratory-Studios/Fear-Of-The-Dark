#include "QuestTypes.h"

#include <fstream>

#include "LuaScript.h"
#include "ScripterMain.h"

namespace QuestModule {
    Objective::Objective(std::string& instructions, std::string& confirmationLuaLiteralOrPath, bool luaInFile/* = false*/) {
        m_instructions = instructions;

        if(luaInFile) {
            // Time to read the file and determine the literal.
            std::ifstream file(confirmationLuaLiteralOrPath);

            std::string line = "";
            while(std::getline(file, line)) {
                m_luaLiteral += line + "\n";
            }
        } else {
            // The Lua is already literal
            m_luaLiteral = confirmationLuaLiteralOrPath;
        }
    }

    void Objective::start(ScriptingModule::Scripter* scripter) {
        /// Starts the confirmation "script", grabbing it's LuaScript pointer.
        m_confirmationScript = scripter->executeCommand(m_luaLiteral);
    }
    bool Objective::update() {
        /// Checks if confirmation "script" finished.
        if(m_confirmationScript->isFinished()) {
            return true;
        }
        return false;
    }

    std::string Objective::getDisplayText() {
        return m_instructions;
    }

    std::string Objective::getConfirmationLuaLiteral() {
        return "None-o-ya-business!";
    }

    Quest::Quest(unsigned int id) {
        XML_QuestData data = XMLData::getQuestData(id);

        initObjectives(data.objectives);

        if(data.scriptIsFile) {
            // Time to read the file and determine the literal.
            std::ifstream file(data.completionScript);

            std::string line = "";
            while(std::getline(file, line)) {
                m_completionLuaLiteral += line + "\n";
            }
        } else {
            // The Lua is already literal
            m_completionLuaLiteral = data.completionScript;
        }
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
            if(!startObjective(scripter, current[current.size()-1]+1)) {
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
        scripter->executeCommand(m_completionLuaLiteral);
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

    void Quest::initObjectives(std::vector<XML_QuestObjectiveData> objectiveData) {
        /// Initializes all objectives based on their XML_Objective objects.
        for(unsigned int i = 0; i < objectiveData.size(); i++) {
            m_objectives.emplace_back(objectiveData[i].text, objectiveData[i].confirmationScript, objectiveData[i].scriptIsFile);
            m_objectiveStates.emplace_back(ObjectiveState::HIDDEN);
        }
    }
}
