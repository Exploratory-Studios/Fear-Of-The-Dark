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
        void updateLimbs() {}

        Leg* m_leg0 = nullptr;
};

class EntityBaseProjectile : public Entity { // ABSTRACT
    public:
        EntityBaseProjectile(glm::vec2 pos, AudioManager* audioManager, float damage, bool gravity = true);
        void onInteract(ScriptQueue* sq) = 0;
        void onDeath(ScriptQueue* sq) = 0;
    protected:
        void updateAI() { m_position += m_velocity * m_speed; }
        void updateLimbs() {}
        float m_damage;
        bool m_gravity = false;
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
        void updateLimbs() {}
};

enum class Type {
    INTEGER,
    FLOAT,
    STRING,
    POINTER,
    BOOLEAN
};

union P {
    int Integer;
    float Float;
    char* C_String;
    void* Pointer;
    bool Boolean;
};

class Parameter {
public:
    void setInt(int setting) { p.Integer = setting; t = Type::INTEGER; }
    void setFloat(float setting) { p.Float = setting; t = Type::FLOAT; }
    void setCString(char* setting) { p.C_String = setting; t = Type::STRING; }
    void setPointer(void* setting) { p.Pointer = setting; t = Type::POINTER; }
    void setBool(bool setting) { p.Boolean = setting; t = Type::BOOLEAN; }

    P p;
    Type t;
};

#include <iostream>
static Entity* createEntity(unsigned int id, glm::vec2 position, AudioManager* audioManager, std::vector<Parameter> extras) {
    /*int extraArgs = 0; FOR EXTRA ARGUMENTS
    va_list args;
    va_start(args, extraArgs);

    va_end(args);*/

    Entity* ret = nullptr;
    switch(id) {
        case (unsigned int)Categories::EntityIDs::MOB_PLAYER: {
            // extras: inputmanager, scriptqueue
            ret = new Player(position, reinterpret_cast<GLEngine::InputManager*>(extras[0].p.Pointer), reinterpret_cast<ScriptQueue*>(extras[1].p.Pointer));
            break;
        }
        case (unsigned int)Categories::EntityIDs::MOB_NEUTRAL_COMPANIONCUBE: {
            ret = new EntityNeutralCompanionCube(position, audioManager);
            break;
        }
        case (unsigned int)Categories::EntityIDs::MOB_NEUTRAL_QUESTGIVER_A: {
            // extras: QuestManager
            ret = new EntityNeutralQuestGiverA(position, audioManager, reinterpret_cast<QuestManager*>(extras[0].p.Pointer));
            break;
        }
    }

    return ret;
}

/*static std::vector<Parameter> getEntityParameters(unsigned int id) {
    std::vector<Parameter> ret;

    switch(id) {
        case (unsigned int)Categories::EntityIDs::MOB_PLAYER: {
            Parameter p;
            p.setPointer(nullptr, PointerType::INPUT_MANAGER);
            ret.push_back(p);
            p.setPointer(nullptr, PointerType::SCRIPT_QUEUE);
            ret.push_back(p);
            break;
        }
        case (unsigned int)Categories::EntityIDs::MOB_NEUTRAL_QUESTGIVER_A: {
            Parameter p;
            p.setPointer(nullptr, PointerType::QUEST_MANAGER);
            ret.push_back(p);
            break;
        }
    }

    return ret;
}*/
