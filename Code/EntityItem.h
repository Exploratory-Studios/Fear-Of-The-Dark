#pragma once

#include "Entity.h"


class EntityItem : public Entity
{
    public:
        EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id, MetaData data, bool loadTex);
        EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id, MetaData data, bool loadTex);
        virtual ~EntityItem();

        /// Getters
        Item* getItem() { return nullptr; } /// TODO: Do this

    protected:
        // Item
        unsigned int m_itemId = (unsigned int)-1;
};
