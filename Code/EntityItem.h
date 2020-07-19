#pragma once

#include "Entity.h"

#include "Item.h"

class EntityItem : public Entity {
	public:
		EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
		EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id, SaveDataTypes::MetaData data, bool loadTex);
		virtual ~EntityItem();

		void init();

		virtual bool collideWithOther(Entity* other) override;

		/// Getters
		Item* getItem();
		unsigned int getItemID() {
			return m_itemId;
		}

	protected:
		// Item
		unsigned int m_itemId = (unsigned int) - 1;
};
