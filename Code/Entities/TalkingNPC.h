#pragma once

#include "../Entity.h"

#include "../QuestClasses.h"

class TalkingNPC : public Entity
{
    friend class EntityManager;

    public:
        TalkingNPC(glm::vec2 position, unsigned int id);
        ~TalkingNPC();

        void startDialogue();

        bool isDialogueStarted() { return m_dialogueStarted; }
        unsigned int getQuestionId() { return m_questionId; }

    private:
        unsigned int m_questionId = 0;

        bool m_dialogueStarted = false;

};
