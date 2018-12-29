#pragma once

#include <SpriteBatch.h>
#include <vector>

#include "Entity.h"
#include "Chunk.h"

#include "EntityManager.h"
#include "WorldIOManager.h"
#include "QuestManager.h"

#include "Scripting/ScripterMain.h"

#include "PresetValues.h"

class WorldManager
{
    friend class Scripter;

    public:
        WorldManager();
        virtual ~WorldManager();

        void init(WorldIOManager* worldIOManager);

        void update(GLEngine::Camera2D* worldCamera, float timeStepVariable, float time);
        void tick(float tickTime);
        void draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime, GLEngine::GLSLProgram* program);

        Player* getPlayer() { return m_player; }

    private:
        void activateChunks();

        EntityManager m_entityManager;
        WorldIOManager* m_worldIOManager = nullptr;
        QuestManager* m_questManager = nullptr;

        Player* m_player;                // NO... Why not? I think I may possibly, with just a slight chance, be a pyschopath

        std::vector<int> m_activatedChunks;
        int m_lastActivated = -1;
};
