#pragma once

#include "Entity.h"
#include "Entities/TalkingNPC.h"

class EntityManager
{
    public:
        EntityManager();
        virtual ~EntityManager();

        void update(float timeStep, Chunk* chunks[WORLD_SIZE]); // Updates entities (AI, parentChunk, activatedState)
        void draw(GLEngine::SpriteBatch& sb, float time); // Draws entities
        void tick(); // Spawns entities

        void addEntity(Entity* ent) { m_entities.push_back(ent); }
        void addTalkingEntity(TalkingNPC* ent) { m_entities.push_back(ent); m_talkingEntities.push_back(ent); }

        std::vector<Entity*> getEntities() { return m_entities; }

    private:
        std::vector<Entity*> m_entities; // All entities, including TalkingNPCs
        std::vector<TalkingNPC*> m_talkingEntities;  // Pointers to talkingNpcs to preserve special data.

        Chunk* m_parentChunk = nullptr;

};
