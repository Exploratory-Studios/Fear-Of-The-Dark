#include "Quest.h"

Quest::Quest(std::vector<Objective*> objectives, std::function<void(Player*)> rewardFunction) : m_objectives(objectives), m_rewardFunction(rewardFunction)
{

}

Quest::~Quest()
{
    for(unsigned int i = 0; i < m_objectives.size(); i++) {
        delete m_objectives[i];
    }
}
