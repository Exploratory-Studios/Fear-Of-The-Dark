#pragma once

#include <vector>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "PresetValues.h"
#include "Item.h"

class Inventory
{
    friend class WorldIOManager;
    public:
        Inventory();
        virtual ~Inventory() {}

        bool addItem(Item* newItem);
        void subtractItem(Item* item);

        Item* getItem(unsigned int x) { if(m_items.size() > x) { return m_items[x]; } else { return nullptr; } }
        unsigned int getItemIndex(Item* item) { if(item) { for(unsigned int i = 0; i < m_items.size(); i++) { if(m_items[i]->getID() == item->getID() && m_items[i]->getQuantity() >= item->getQuantity()) return i; } } return (unsigned int)-1; }
        std::vector<Item*> getItems() { return m_items; }
        float getCurrentWeight();
        float getSpeedMultiplier();

        void updateWeight();
        void update();
        void draw(float x, float y, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);

        SaveDataTypes::InventoryData getInventorySaveData();

    private:
        float m_weight = 0.0f; // The average person can carry about 40-45lbs of weight and still walk normally
        // The max weight at which you can still walk the normal speed, defined in PRESETS file. Called MAX_WEIGHT
        float m_absMaxWeight = 75.0f; // The weight at which you can't walk anymore (around 70-75lbs). 2x the max weight at normal walking speed
        std::vector<Item*> m_items = {};
};
