#pragma once

#include <unordered_map>

#include "QuestTypes.h"

namespace QuestModule {
    class QuestManager
    {
        /** Responsibilities:
             - Store all active and finished quests
             - Enable the Scripter to begin quests
             - Update Quests
             - Store and allow modification of flags
        **/
        public:
            QuestManager(ScriptingModule::Scripter* scripter);

            void startQuest(unsigned int id); // Retrieves (from XMLData) and starts a quest with id `id`

            void update();

            void setFlag(unsigned int id, bool value); // Sets a flag to a value of `value`
            bool getFlag(unsigned int id) const; // Returns the bool value of a flag with id `id`

            std::vector<Quest> getCurrentQuests(); // Returns all current quests
            std::vector<Quest> getFinishedQuests(); // Returns all finished quests

        private:
            ScriptingModule::Scripter* m_scripter;

            std::vector<Quest> m_currentQuests; // Quests to update
            std::vector<Quest> m_finishedQuests; // Quests that are finished

            std::unordered_map<unsigned int, bool> m_flags; // Map, so that new ones can be added as needed, and randomly accessed. Unordered, because it has faster insertions and random access.
    };
}
