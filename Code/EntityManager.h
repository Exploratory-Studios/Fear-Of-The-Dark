#pragma once

#include "Entity.h"
#include "Player.h"

class Chunk;

class EntityManager
{
    friend class Scripter;
    public:
        EntityManager(Chunk* parent, AudioManager* audio);
        virtual ~EntityManager();

        void update(float timeStep, Chunk* chunks[WORLD_SIZE]); // Updates entities (AI, parentChunk, activatedState)
        void draw(GLEngine::SpriteBatch& sb, float time, float xOffset); // Draws entities
        void tick(Player* p, float tickTime, WorldEra& era);

        void addEntity(Entity ent) { m_entities->push_back(ent); }

        void removeEntity(int index);

        //std::vector<Entity*> getEntities() { return m_entities; } // DEPRECATED
        Entity* getEntity(unsigned int index) { return (Entity*)&(*m_entities)[index]; }
        unsigned int getNumEntities() { return m_entities->size(); }
        std::vector<Entity>* getEntities() { return m_entities; }

        void targetEntities(Player* p);
        std::vector<glm::vec2> pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition);

    private:
        void spawnEntities(float tickTime, WorldEra& era);

        std::vector<Entity>* m_entities; // All entities, excluding TalkingNPCs

        Chunk* m_parentChunk = nullptr;
        AudioManager* m_audioManager = nullptr;

};
