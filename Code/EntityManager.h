#pragma once

#include "Entity.h"
#include "Entities/TalkingNPC.h"

//class Chunk;

class EntityManager
{
    public:
        EntityManager();
        virtual ~EntityManager();

        void update(float timeStep); // Updates entities (AI, parentChunk, activatedState)
        void draw(GLEngine::SpriteBatch& sb, float time); // Draws entities
        void tick(); // Spawns entities

        bool isDialogueStarted();

    private:
        std::vector<Entity*> m_entities;
        std::vector<TalkingNPC*> m_talkingEntities;

        Chunk* m_parentChunk = nullptr;

};
