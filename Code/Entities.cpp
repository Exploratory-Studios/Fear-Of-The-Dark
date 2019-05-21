#include "Entities.h"

EntityNeutralCompanionCube::EntityNeutralCompanionCube(glm::vec2 pos, AudioManager* audioManager) : Entity(pos, audioManager, nullptr) {
    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Mobs/Mob0.png");
    m_size = glm::vec2(1.0f, 1.0f);
    m_faction = Categories::Faction::BAD;
    m_jumpHeight = 2.608f;
    m_speed = 0.4f;
    m_ai = Categories::AI_Type::WALKING;
    m_disabilities = Categories::Disability_Type::NONE;
    m_attackType = Categories::Attack_Type::NONE;
    m_transparent = false;
    /// LIMBS (TODO)
}

EntityBaseProjectile::EntityBaseProjectile(glm::vec2 pos, AudioManager* audioManager, float damage, bool gravity /*= true*/) : Entity(pos, audioManager, nullptr) {
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

EntityBaseQuestGiver::EntityBaseQuestGiver(glm::vec2 pos, AudioManager* audioManager, QuestManager* qm, unsigned int questionId) : Entity(pos, audioManager, nullptr) {
    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Mobs/Mob0.png");
    m_size = glm::vec2(1.0f, 2.0f);
    m_faction = Categories::Faction::NEUTRAL;
    m_jumpHeight = 2.608f;
    m_speed = 0.1f;
    m_ai = Categories::AI_Type::WALKING;
    m_disabilities = Categories::Disability_Type::NONE;
    m_attackType = Categories::Attack_Type::NONE;
    m_transparent = false;

    m_qm = qm;
    m_questionId = questionId;
}

void EntityBaseQuestGiver::onInteract(ScriptQueue* sq) {
    if(m_qm) {
        m_qm->setQuestionId(m_questionId);
        m_qm->setDialogueStarted(true);
    }
}
