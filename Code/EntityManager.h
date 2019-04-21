#pragma once

#include "Entity.h"
#include "Entities/TalkingNPC.h"
#include "Player.h"

class Chunk;

class EntityManager
{
    friend class Scripter;
    public:
        EntityManager(Chunk* parent);
        virtual ~EntityManager();

        void update(float timeStep, Chunk* chunks[WORLD_SIZE]); // Updates entities (AI, parentChunk, activatedState)
        void draw(GLEngine::SpriteBatch& sb, float time, float xOffset); // Draws entities
        void tick(Player* p); // Spawns entities

        void addEntity(Entity* ent) { m_entities.push_back(ent); }
        void addTalkingEntity(TalkingNPC* ent) { m_talkingEntities.push_back(ent); }

        void removeEntity(int index);
        void removeTalkingEntity(int index);

        std::vector<TalkingNPC*> getTalkingEntities() { return m_talkingEntities; }
        std::vector<Entity*> getEntities() { return m_entities; }
        std::vector<Entity*> getAllEntities() {
            std::vector<Entity*> allEnts;
            for(unsigned int i = 0; i < m_entities.size(); i++) {
                allEnts.push_back(m_entities[i]);
            }
            for(unsigned int i = 0; i < m_talkingEntities.size(); i++) {
                allEnts.push_back(m_talkingEntities[i]);
            }
            return allEnts;
        }

        void targetEntities(Player* p);
        std::vector<glm::vec2> pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition);

    private:
        std::vector<Entity*> m_entities; // All entities, excluding TalkingNPCs
        std::vector<TalkingNPC*> m_talkingEntities;  // Pointers to talkingNpcs to preserve special data.

        Chunk* m_parentChunk = nullptr;

};
