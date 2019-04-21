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
        ~Inventory() {}

        bool addItem(Item newItem);

        Item* getItem(int x) { if(m_items.size() > x) { return &m_items[x]; } else { return nullptr; } }
        std::vector<Item>* getItems() { return &m_items; }
        float getCurrentWeight();
        float getSpeedMultiplier();

        void updateWeight();
        void update();
        void draw(float x, float y, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);

    private:
        float m_weight = 0.0f; // The average person can carry about 40-45lbs of weight and still walk normally
        // The max weight at which you can still walk the normal speed, defined in PRESETS file. Called MAX_WEIGHT
        float m_absMaxWeight; // The weight at which you can't walk anymore (around 70-75lbs). 2x the max weight at normal walking speed
        std::vector<Item> m_items;
};
