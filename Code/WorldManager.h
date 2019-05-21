#pragma once

#include <SpriteBatch.h>
#include <vector>
#include <ParticleEngine2D.h>

#include "Entity.h"
#include "Chunk.h"

#include "WorldIOManager.h"
#include "QuestManager.h"
#include "AudioManager.h"

#include "PresetValues.h"

class WorldManager
{
    friend class Scripter;

    public:
        WorldManager();
        virtual ~WorldManager();

        void init(WorldIOManager* worldIOManager, GLEngine::ParticleEngine2D* particle2d);

        void update(GLEngine::Camera2D* worldCamera, float timeStepVariable, float time, GLEngine::InputManager& input, GLEngine::GUI* gui);
        void tick();
        void draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime, GLEngine::GLSLProgram* program);
        void drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, GLEngine::GUI* gui);

        Player* getPlayer() { return m_player; }

        bool isDialogueStarted() { return m_questManager->isDialogueStarted(); }
        bool isDialogueActive() { return m_questManager->isDialogueActive(); }

        void startDialogue();

    private:
        void activateChunks();

        QuestManager* m_questManager = nullptr;
        WorldIOManager* m_worldIOManager = nullptr;
        AudioManager m_audioManager;

        Player* m_player;

        std::vector<int> m_activatedChunks;
        int m_lastActivated = -1;
};
