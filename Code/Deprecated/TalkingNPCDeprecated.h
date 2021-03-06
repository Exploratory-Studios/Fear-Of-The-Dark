/*#pragma once

#include "../Entity.h"

#include "../QuestClasses.h"

class TalkingNPC : public Entity
{

    public:
        TalkingNPC(glm::vec2 position, unsigned int id, AudioManager* audioManager);
        ~TalkingNPC();

        void startDialogue();

        void draw(GLEngine::SpriteBatch& sb, float time, float xOffset) override;

        bool isDialogueStarted() { return m_dialogueStarted; }
        void setDialogueStarted(bool setting) { m_dialogueStarted = setting; }

        unsigned int getQuestionId() { return m_questionId; }

    private:
        unsigned int m_questionId = 0;

        bool m_dialogueStarted = false;

};
*/
