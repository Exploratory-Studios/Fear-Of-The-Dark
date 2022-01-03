#pragma once

#include "Entity.h"

#include "Item.h"

class EntityItem : public Entity {
  public:
	EntityItem(glm::vec2 pos, unsigned int layer, unsigned int id);
	EntityItem(glm::vec2 pos, unsigned int layer, EntityIDs id);
	virtual ~EntityItem();

	void init();
	void init(SaveDataTypes::EntityItemData& data);

	virtual bool collideWithOther(Entity* other) override;

	/// Getters
	Item*		 getItem();
	unsigned int getItemID() {
		return m_itemId;
	}

  protected:
	// Item
	unsigned int m_itemId = (unsigned int)-1;
};
