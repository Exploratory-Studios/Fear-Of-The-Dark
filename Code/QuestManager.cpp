#include "QuestManager.h"

#include "LuaScript.h"
#include "ScripterMain.h"
#include "EventQueue.h"

namespace QuestModule {

    QuestManager::QuestManager(ScriptingModule::Scripter* scripter) : m_scripter(scripter) {
        // Nothing really to put here. It's just a manager.
    }

    void QuestManager::startQuest(unsigned int id) {
        /// Retrieves (rather, the Quest object retrieves its own data) and starts a quest with id `id`

        Quest quest(id);
        quest.start(m_scripter);

        m_currentQuests.push_back(quest);
    }

    void QuestManager::update() {
        /// Updates all current scripts.
        for(unsigned int i = 0; i < m_currentQuests.size(); i++) {
            m_currentQuests[i].update(m_scripter);
        }

        std::vector<ScriptingModule::Argument> args;
        EventModule::EventQueue::triggerEvent("update", args, false);
    }

    void QuestManager::setFlag(unsigned int id, bool value) {
        /// Sets a flag to a value of `value`
        m_flags[id] = value;

        std::vector<ScriptingModule::Argument> args = { ScriptingModule::Argument("flagID", std::to_string(id)),
                                                        ScriptingModule::Argument("flagValue", value ? "1" : "0") };
        EventModule::EventQueue::triggerEvent("setFlag", args);
    }

    bool QuestManager::getFlag(unsigned int id) const {
        /// Returns the bool value of a flag with id `id`
        bool val = false;

        try {
            val = m_flags.at(id);
        } catch(const std::out_of_range& err) {
            Logger::getInstance()->log("WARNING: An attempt to access a flag with ID: " + std::to_string(id) + " failed! (Flag doesn't exist)", true);
        }

        return val;
    }

    std::vector<Quest> QuestManager::getCurrentQuests() {
        /// Returns all current quests
        return m_currentQuests;
    }

    std::vector<Quest> QuestManager::getFinishedQuests() {
        /// Returns all finished quests
        return m_finishedQuests;
    }
}
