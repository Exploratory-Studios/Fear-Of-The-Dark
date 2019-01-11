#pragma once

#include "Entity.h"
#include "Entities/TalkingNPC.h"
#include "Player.h"

class Chunk;

class EntityManager
{
    public:
        EntityManager(Chunk* parent);
        virtual ~EntityManager();

        void update(float timeStep, Chunk* chunks[WORLD_SIZE]); // Updates entities (AI, parentChunk, activatedState)
        void draw(GLEngine::SpriteBatch& sb, float time, float xOffset); // Draws entities
        void tick(Player* p); // Spawns entities

        void addEntity(Entity* ent) { m_entities.push_back(ent); }
        void addTalkingEntity(TalkingNPC* ent) { m_entities.push_back(ent); m_talkingEntities.push_back(ent); }

        std::vector<Entity*> getEntities() { return m_entities; }

        void targetEntities(Player* p);
        std::vector<glm::vec2> pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition);

    private:
        std::vector<Entity*> m_entities; // All entities, including TalkingNPCs
        std::vector<TalkingNPC*> m_talkingEntities;  // Pointers to talkingNpcs to preserve special data.

        Chunk* m_parentChunk = nullptr;

};
