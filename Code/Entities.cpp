#include "Entities.h"

EntityNeutralItem::EntityNeutralItem(glm::vec2 pos, unsigned int layer, Item* item) : Entity(pos, layer, 0.0f, Categories::LootTableIds::NONE, 0, 0)
{
    m_texture = item->getTexture();
    m_size = glm::vec2(1.0f);
    m_faction = Categories::Faction::NEUTRAL;
    m_type = Categories::Entity_Type::ITEM;
    m_jumpHeight = 0.0f;
    m_speed = 0.0f;
    m_ai = Categories::AI_Type::NONE;
    m_disabilities = Categories::Disability_Type::NONE;
    m_transparent = false;

    m_canDie = false;
    m_takesFallDamage = false;

    m_item = item;
}

EntityNeutralCompanionCube::EntityNeutralCompanionCube(glm::vec2 pos, unsigned int layer) : Entity(pos, layer, 12.0f/60.0f, Categories::LootTableIds::NONE, 0, 0) {
    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Mobs/Mob0.png");
    m_size = glm::vec2(1.0f, 1.0f);
    m_faction = Categories::Faction::BAD;
    m_jumpHeight = 0.4f;
    m_speed = 0.3f;
    m_ai = Categories::AI_Type::WALKING;
    m_disabilities = Categories::Disability_Type::NONE;
    m_attackType = Categories::Attack_Type::NONE;
    m_transparent = false;

    m_lootRolls = 10;


    /*m_leg0 = new Leg(this, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/LogPile.png").id, glm::vec2(0.7f, 0.8f), glm::vec2(0.3f, 0.8f), 0.0f, 3.14f);

    m_limbs.push_back(m_leg0);
    /// LIMBS (TODO) USE ENTITYDATA (PresetValues.h)*/
}

EntityBaseProjectile::EntityBaseProjectile(glm::vec2 pos, unsigned int layer, float damage, bool gravity /*= true*/) : Entity(pos, layer, MAX_SPEED, Categories::LootTableIds::NONE, 0, 0) {
    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Projectiles/ProjectileBase.png");
    m_size = glm::vec2(0.1f, 0.1f);
    m_faction = Categories::Faction::NEUTRAL;
    m_jumpHeight = 0.0f;
    m_speed = 0.2f;
    m_ai = Categories::AI_Type::NONE;
    m_disabilities = Categories::Disability_Type::NONE;
    m_attackType = Categories::Attack_Type::CONTACT;
    m_transparent = false;

    m_damage = damage;
    m_gravity = gravity;
}

void EntityBaseSpeaker::onTalk(ScriptQueue* sq) {
    if(/*m_qm && */m_questionId != (unsigned int) -1) {
        //m_qm->setQuestionId(m_questionId);
        //m_qm->setDialogueStarted(true);

        Script s;
        s.commands.push_back("startDialogue " + std::to_string(m_questionId));
        int a = sq->addScript(s);
        sq->activateScript(a);
    }
}

void EntityBaseSpeaker::onTrade(ScriptQueue* sq) {
    if(/*m_qm && */m_tradeTableId != (unsigned int) -1) {
        //m_qm->setTradeTableId(m_tradeTableId);
        //m_qm->setTradingStarted(true);

        Script s;
        s.commands.push_back("startTrade " + std::to_string(m_tradeTableId));
        int a = sq->addScript(s);
        sq->activateScript(a);
    }
}


EntityBaseQuestGiver::EntityBaseQuestGiver(glm::vec2 pos,
                                           unsigned int layer,
                                           float maxRunSpeed,
                                           Categories::LootTableIds lootTable,
                                           unsigned int lootBeginLevel,
                                           unsigned int lootBeginIndex,
                                           unsigned int questionId) : EntityBaseSpeaker(pos,
                                                                                        layer,
                                                                                        maxRunSpeed,
                                                                                        lootTable,
                                                                                        lootBeginLevel,
                                                                                        lootBeginIndex,
                                                                                        questionId,
                                                                                        0) {

    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Mobs/Mob0.png");
    m_size = glm::vec2(1.0f, 2.0f);
    m_faction = Categories::Faction::EVIL;
    m_jumpHeight = 0.4f;
    m_speed = 0.01f;
    m_ai = Categories::AI_Type::WALKING;
    m_disabilities = Categories::Disability_Type::NONE;
    m_attackType = Categories::Attack_Type::NONE;
    m_transparent = false;
    m_maxSpeed = maxRunSpeed;

    m_canDie = false;

    m_lootRolls = 10;

    m_makesNoise = true;
    m_noiseFrequency = 1; // 1/100 ticks, this mob will make noise
    m_ambientNoiseSound.push_back(SoundEffectIDs::HUSK_SOUND);
}
