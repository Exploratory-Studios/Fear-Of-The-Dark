#pragma once

#include "Entity.h"

class EntityProjectile : public Entity
{
    public:
        EntityProjectile(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
        EntityProjectile(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex);
        virtual ~EntityProjectile();

    protected:
        // Movement
        float m_speed;
        bool m_collideWithBlocks;

        // Combat
        float m_damage;

};
