#include "TalkingNPC.h"

TalkingNPC::TalkingNPC(glm::vec2 position, unsigned int id) {
    unsigned int mobId = Category_Data::dialogueMobData[id].mobId;
    m_questionId = Category_Data::dialogueMobData[id].dialogueId;

    init(position, Categories::Entity_Type::MOB, mobId);
}

TalkingNPC::~TalkingNPC()
{
    //dtor
}

void TalkingNPC::startDialogue() {
    m_dialogueStarted = true;
}
