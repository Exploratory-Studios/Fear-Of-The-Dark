#pragma once

#include "../Player.h"

enum class OBJECTIVE_TYPE {
    KILL,
    RETRIEVE,
    GOTO,
    TALK,
    GIVE
};

enum class REWARD_TYPE {
    XP = 0x1,
    MONEY = 0x2,
    EVENT = 0x4,
    REPUTATION = 0x8,
    NONE = 0x0
};

class Objective
{
    public:
        Objective(OBJECTIVE_TYPE objectiveType, bool rewardTypes) : m_objectiveType(objectiveType), m_rewardTypes(rewardTypes) {}
        ~Objective() {}

        virtual bool isComplete(...) = 0; // We can predict what arguments we'll get by the type of objective

        const bool              getRewardTypes()    const { return m_rewardTypes; }
        const OBJECTIVE_TYPE    getObjectiveType()  const { return m_objectiveType; }
        const std::string       getObjectiveText()  const { return m_objectiveText; }
    private:
        OBJECTIVE_TYPE m_objectiveType;
        bool m_rewardTypes;

        std::string m_objectiveText; // Example: "Kill Jeff" or "Finish the job" etc.
};
