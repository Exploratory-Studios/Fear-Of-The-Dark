#include "TalkingNPC.h"

TalkingNPC::TalkingNPC(glm::vec2 position, unsigned int id) {
    unsigned int mobId = Category_Data::dialogueMobData[id].mobId;
    m_questionId = Category_Data::dialogueMobData[id].dialogueId;

    m_dialogueActive = false;
    m_dialogueStarted = false;

    init(position, Categories::Entity_Type::MOB, mobId);
}

TalkingNPC::~TalkingNPC()
{
    //dtor
}

void TalkingNPC::startDialogue() {
    if(!m_dialogueActive) {
        m_dialogueStarted = true;
        m_dialogueActive = false;
    }
}
