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

        void init(Player& entity, std::vector<Entity>& entities);

        void update(std::vector<Chunk>& activatedChunks);

        Player*                  getPlayer()     { if(m_player) return m_player;      }
        std::vector<Entity*>     getEntities()   { if(!m_entities.empty()) return m_entities;    }

    private:
        void collideEntities(std::vector<Chunk>& activatedChunks);
        void moveEntities();
        void spawnEntities();

        Player* m_player = nullptr;
        std::vector<Entity*> m_entities = { nullptr };
};
