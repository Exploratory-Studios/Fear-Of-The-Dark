#pragma once

#include <vector>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "PresetValues.h"
#include "Item.h"

class Inventory
{
    public:
        Inventory();

        bool addItem(Item newItem, int x = -1, int y = -1);

        Item& getItem(int x, int y) { return m_items[y][x]; }

        void updateWeight();

        float getCurrentWeight();
        float getSpeedMultiplier();

        void draw(float x, float y, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);

    private:
        Item m_items[INVENTORY_BOX_NUM_Y][INVENTORY_BOX_NUM_X];
        float m_weight; // The average person can carry about 40-45lbs of weight and still walk normally
        // The max weight at which you can still walk the normal speed, defined in PRESETS file. Called MAX_WEIGHT
        float m_absMaxWeight; // The weight at which you can't walk anymore (around 70-75lbs). 2x the max weight at normal walking speed
};
