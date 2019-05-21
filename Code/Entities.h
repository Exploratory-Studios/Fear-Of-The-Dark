#pragma once

#include "Entity.h"
#include "Player.h"

#include <stdarg.h>

/* Entities.h attributes:
- Texture
- Faction
- JumpHeight
- Position
- Size
- Transparency
- AI Type
- Disability Types
- Attack Types
- Limbs


m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Mobs/Mob0.png");
m_size = glm::vec2(1.0f, 2.0f);
m_faction = Categories::Faction::GOOD;
m_jumpHeight = 2.608f;
m_speed = 0.2f;
//m_ai = Categories::AI_Type::;
//m_disabilities = Categories::Disability_Type::NONE;
//m_attackType = Categories::Attack_Type::;
m_transparent = false;
/// LIMBS (TODO)
*/

class EntityNeutralCompanionCube : public Entity {
    public:
        EntityNeutralCompanionCube(glm::vec2 pos, AudioManager* audioManager);
        void onInteract(ScriptQueue* sq) { }
        void onDeath(ScriptQueue* sq) { }
    protected:
        void updateAI() { EntityFunctions::WalkingAI(m_controls, m_targets, m_curTarget, m_velocity, m_size, m_position); }
};

class EntityBaseQuestGiver : public Entity { // ABSTRACT
    public:
        EntityBaseQuestGiver(glm::vec2 pos, AudioManager* audioManager, QuestManager* qm, unsigned int questionId);
        void onInteract(ScriptQueue* sq);
        virtual void post_onInteract(ScriptQueue* sq) = 0;
        void onDeath(ScriptQueue* sq) = 0;
    protected:
        QuestManager* m_qm = nullptr;
        unsigned int m_questionId = 0;

        void updateAI() = 0;
};

class EntityNeutralQuestGiverA : public EntityBaseQuestGiver {
    public:
        EntityNeutralQuestGiverA(glm::vec2 pos, AudioManager* audioManager, QuestManager* qm) : EntityBaseQuestGiver(pos, audioManager, qm, 0) { }
        void post_onInteract(ScriptQueue* sq) { }
        void onDeath(ScriptQueue* sq) { }
    protected:
        void updateAI() { EntityFunctions::WalkingAI(m_controls, m_targets, m_curTarget, m_velocity, m_size, m_position); }
};
#include <iostream>
static Entity* createEntity(unsigned int id, glm::vec2 position, AudioManager* audioManager, ...) {
    /*int extraArgs = 0; FOR EXTRA ARGUMENTS
    va_list args;
    va_start(args, extraArgs);

    va_end(args);*/

    Entity* ret = nullptr;
    switch(id) {
        case (unsigned int)Categories::EntityIDs::MOB_PLAYER: {
            int extraArgs = 2;
            va_list args;
            va_start(args, extraArgs);
            ret = new Player(position, va_arg(args, GLEngine::InputManager*), va_arg(args, ScriptQueue*));
            va_end(args);
            break;
        }
        case (unsigned int)Categories::EntityIDs::MOB_NEUTRAL_COMPANIONCUBE: {
            ret = new EntityNeutralCompanionCube(position, audioManager);
            break;
        }
        case (unsigned int)Categories::EntityIDs::MOB_NEUTRAL_QUESTGIVER_A: {
            int extraArgs = 1;
            va_list args;
            va_start(args, extraArgs);
            ret = new EntityNeutralQuestGiverA(position, audioManager, va_arg(args, QuestManager*));
            va_end(args);
            break;
        }
    }

    return ret;
}
