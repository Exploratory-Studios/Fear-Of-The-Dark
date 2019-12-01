#pragma once

#include "Entity.h"
#include "Player.h"

#include <stdarg.h>

#include "Logging.h"

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
/// LIMBS (TODO (delete, we're not using it))
*/

class EntityNeutralItem : public Entity {
    public:
        EntityNeutralItem(glm::vec2 pos, unsigned int layer, Item* item);
        ~EntityNeutralItem() { }
        void onDeath(ScriptQueue* sq) { }

        void setItem(Item* item) { m_item = item; }
        Item* getItem() { return m_item; }
    private:
        void updateAI() { }
        void updateLimbs() { }

        Item* m_item = nullptr;
};

class EntityNeutralCompanionCube : public Entity {
    public:
        EntityNeutralCompanionCube(glm::vec2 pos, unsigned int layer);
        void onDeath(ScriptQueue* sq) { }
    protected:
        void updateAI() { EntityFunctions::WalkingAI(m_controls, m_targets, m_curTarget, m_velocity, m_size, glm::vec3(m_position.x, m_position.y, m_layer)); }
        void updateLimbs() {}

        Leg* m_leg0 = nullptr;
};

class EntityBaseProjectile : public Entity { // ABSTRACT
    public:
        EntityBaseProjectile(glm::vec2 pos, unsigned int layer, float damage, bool gravity = true);
        void onDeath(ScriptQueue* sq) = 0;
    protected:
        void updateAI() { m_position += m_velocity * m_speed; }
        void updateLimbs() {}
        float m_damage;
        bool m_gravity = false;
};

class EntityBaseSpeaker : public Entity { // ABSTRACT
    public:
        EntityBaseSpeaker(glm::vec2 pos,
                          unsigned int layer,
                          float maxRunningSpeed,
                          Categories::LootTableIds lootTable,
                          unsigned int lootTableStartLevel,
                          unsigned int lootTableStartIndex,
                          unsigned int questionId,
                          unsigned int tradeTableId) :
                            Entity(pos,
                                   layer,
                                   maxRunningSpeed,
                                   lootTable,
                                   lootTableStartLevel,
                                   lootTableStartIndex)
                            {
                                m_questionId = questionId;
                                m_tradeTableId = tradeTableId;
                            }
        virtual void onTalk(ScriptQueue* sq); /// TODO: Use scriptQueue to start quests, open UIs, etc.
        virtual void onTrade(ScriptQueue* sq);
        virtual void post_onInteract(ScriptQueue* sq) = 0;
        void onDeath(ScriptQueue* sq) = 0;

        virtual bool canTrade() const override { return (m_tradeTableId != (unsigned int) -1); }
        virtual bool canTalk()  const override { return (m_questionId != (unsigned int) -1);   }
    protected:
        unsigned int m_questionId = (unsigned int) -1;
        unsigned int m_tradeTableId = (unsigned int) -1;

        void updateAI() = 0;
};

class EntityBaseQuestGiver : public EntityBaseSpeaker { // ALSO ABSTRACT
    public:
        EntityBaseQuestGiver(glm::vec2 pos, unsigned int layer, float maxRunSpeed, Categories::LootTableIds lootTable, unsigned int lootBeginLevel, unsigned int lootBeginIndex, unsigned int questionId);
        virtual void post_onInteract(ScriptQueue* sq) = 0;
        void onDeath(ScriptQueue* sq) = 0;
    protected:
        void updateAI() = 0;
};

class EntityNeutralQuestGiverA : public EntityBaseQuestGiver {
    public:
        EntityNeutralQuestGiverA(glm::vec2 pos, unsigned int layer) : EntityBaseQuestGiver(pos, layer, 0.3f, Categories::LootTableIds::ANIMAL, 1, 0, 2) { }
        void post_onInteract(ScriptQueue* sq) { } /// TODO: remove
        void onDeath(ScriptQueue* sq) { }
    protected:
        void updateAI() { EntityFunctions::WalkingAI(m_controls, m_targets, m_curTarget, m_velocity, m_size, glm::vec3(m_position.x, m_position.y, m_layer)); }
        void updateLimbs() {}
};

#include <iostream>
static Entity* createEntity(unsigned int id, glm::vec2 position, unsigned int layer) {

    Entity* ret = nullptr;
    switch(id) {
        case (unsigned int)Categories::EntityIDs::MOB_PLAYER: {
            ret = new Player(position, layer);
            break;
        }
        case (unsigned int)Categories::EntityIDs::MOB_NEUTRAL_COMPANIONCUBE: {
            ret = new EntityNeutralCompanionCube(position, layer);
            break;
        }
        case (unsigned int)Categories::EntityIDs::MOB_NEUTRAL_QUESTGIVER_A: {
            ret = new EntityNeutralQuestGiverA(position, layer);
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
