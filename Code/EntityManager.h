#pragma once

#include <vector>
#include <DebugRenderer.h>

#include "Entity.h"
#include "Player.h"

#include "WorldIOManager.h"
#include "QuestClasses.h"

#include "PresetValues.h"

class EntityManager
{
    public:
        EntityManager();
        virtual ~EntityManager();

        void init(Player* entity, std::vector<Entity*> entities);

        void update(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE], GLEngine::Camera2D* worldCamera, float timeStepVariable);
        void tick(float dayCycleTime, Chunk* chunks[WORLD_SIZE]);
        void draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr, int tickTime, GLEngine::GLSLProgram* program);

        void addTalkingNpc(glm::vec2 position, unsigned int id);

        Player*                  getPlayer()                    { return m_player; }
        std::vector<Entity*>     getEntities()                  { return m_entities; }

        bool                     isDialogueStarted()            { return m_dialogueStarted; }
        TalkingNPC*              getSpeakingNpc()               { return m_speakingNpc; }

    private:
        void collideEntities(std::vector<int>& activatedChunks, Chunk* chunks[WORLD_SIZE]);
        void moveEntities(float timeStepVariable);
        void spawnEntities();
        void targetEntities(Chunk* chunks[WORLD_SIZE]);
        std::vector<glm::vec2> pathfindToTarget(float jumpHeight, glm::vec2 originalPosition, glm::vec2 targetPosition, Chunk* chunks[WORLD_SIZE]);
        void initResponses();

        Player* m_player;
        std::vector<Entity*> m_entities;
        std::vector<TalkingNPC*> m_talkingNpcs;

        bool m_dialogueStarted = false;
        TalkingNPC* m_speakingNpc;
};
