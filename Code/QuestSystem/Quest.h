#pragma once

#include <functional>

#include "Objective.h"
#include "../Player.h"

class Quest
{
    public:
        Quest(std::vector<Objective*> objectives, std::function<void(Player*)> rewardFunction);
        ~Quest();

        void update(); // checks completion of each objective and gives reward if last one is complete

        const Objective* getCurrentObjective() const { if(m_curObjectiveIndex < m_objectives.size()) return m_objectives[m_curObjectiveIndex]; }

    private:
        unsigned short int m_curObjectiveIndex = 0;
        std::vector<Objective*> m_objectives;

        Player* m_player = nullptr;
        //EventManager* m_eventManager* = nullptr;

        std::function<void(Player*)> m_rewardFunction;

};
