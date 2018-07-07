#pragma once

#include <vector>
#include <DebugRenderer.h>

#include "Entity.h"
#include "Player.h"

#include "WorldIOManager.h"

#include "PresetValues.h"

class EntityManager
{
    public:
        EntityManager();
        virtual ~EntityManager();

        void init(Player* entity, std::vector<Entity*> entities);

        void update(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE], GLEngine::Camera2D* worldCamera, float timeStepVariable);
        void tick(float dayCycleTime);
        void draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr);

        Player*                  getPlayer()     { return m_player; }
        std::vector<Entity*>     getEntities()   { return m_entities; }

    private:
        void collideEntities(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE]);
        void moveEntities(float timeStepVariable);
        void spawnEntities();
        void targetEntities();

        Player* m_player;
        std::vector<Entity*> m_entities;
};
