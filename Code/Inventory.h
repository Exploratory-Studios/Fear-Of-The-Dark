#pragma once

#include <vector>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Item.h"

class Inventory
{
    public:
        Inventory();

        void addItem(Item newItem);

        void updateWeight();

        float getCurrentWeight();
        float getSpeedMultiplier();

        void draw(float x, float y, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);

    private:
        std::vector<Item> m_items;
        float m_weight; // The average person can carry about 40-45lbs of weight and still walk normally
        // The max weight at which you can still walk the normal speed, defined in PRESETS file. Called MAX_WEIGHT
        float m_absMaxWeight; // The weight at which you can't walk anymore (around 70-75lbs). 2x the max weight at normal walking speed
};
