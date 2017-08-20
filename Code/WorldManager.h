#pragma once

#include <SpriteBatch.h>
#include <vector>

#include "Entity.h"
#include "Chunk.h"

#include "EntityManager.h"
#include "WorldIOManager.h"

#include "PresetValues.h"

class WorldManager
{
    public:
        WorldManager();
        virtual ~WorldManager();

        void init(WorldIOManager* worldIOManager);

        void update();
        void draw(GLEngine::SpriteBatch& sb);

    private:
        void activateChunks();

        EntityManager m_entityManager;
        WorldIOManager* m_worldIOManager = nullptr;

        Player m_player;                // NO
        std::vector<Entity> m_entities; // COPIES

        Chunk* m_chunks[WORLD_SIZE];

        std::vector<int> m_activatedChunks;
        int m_lastActivated = -1;
};
