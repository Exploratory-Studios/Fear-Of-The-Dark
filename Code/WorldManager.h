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
        void draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr);

        Player* getPlayer() { return m_player; }

    private:
        void activateChunks();

        EntityManager m_entityManager;
        WorldIOManager* m_worldIOManager = nullptr;

        Player* m_player;                // NO

        std::vector<int> m_activatedChunks;
        int m_lastActivated = -1;
};
