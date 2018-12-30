#pragma once

#include "../Entity.h"

#include "../Dialogue.h"

class TalkingNPC : public Entity
{
    public:
        TalkingNPC(glm::vec2 position, unsigned int id);
        ~TalkingNPC();

    private:
        virtual void update(Chunk* chunks[WORLD_SIZE], float timeStep) override;

        Dialogue* m_dialogue = nullptr;

};
