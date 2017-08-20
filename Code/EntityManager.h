#pragma once

#include <vector>

#include "Entity.h"
#include "Player.h"

#include "WorldIOManager.h"

class EntityManager
{
    public:
        EntityManager();
        virtual ~EntityManager();

        void init(Player* entity, std::vector<Entity*> entities);

        void update(std::vector<Chunk>& activatedChunks);
        void draw(GLEngine::SpriteBatch& sb);

        Player*                  getPlayer()     { return m_player; }
        std::vector<Entity*>     getEntities()   { return m_entities; }

    private:
        void collideEntities(std::vector<Chunk>& activatedChunks);
        void moveEntities();
        void spawnEntities();

        Player* m_player;
        std::vector<Entity*> m_entities;
};
