#include "TalkingNPC.h"

TalkingNPC::TalkingNPC(glm::vec2 position, unsigned int id) {
    init(position, Categories::Entity_Type::DIALOGUE, id);
}

TalkingNPC::~TalkingNPC()
{
    //dtor
}

void TalkingNPC::update(Chunk* chunks[WORLD_SIZE], float timeStep) {
    updateAI(chunks);
    updateMovement();
    setParentChunk(chunks);
    updateLightLevel(m_parentChunk);
}
